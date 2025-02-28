#include "PhysicsEngine.h"
#include "PhysUtils.h"

#include "CoffeeEngine/Core/Log.h"
#include "CoffeeEngine/Scene/Components.h"

#include <entt/entity/entity.hpp>


namespace Coffee
{
    std::vector<PhysicsEngine::DebugDrawCommand> PhysicsEngine::debugDrawList;
    using namespace Coffee;

    glm::vec3 PhysicsEngine::GlobalGravity = glm::vec3(0.0f, -9.81f, 0.0f);

    btDynamicsWorld* PhysicsEngine::m_world = nullptr;
    btCollisionConfiguration* PhysicsEngine::m_collision_conf = nullptr;
    btDispatcher* PhysicsEngine::m_dispatcher = nullptr;
    btBroadphaseInterface* PhysicsEngine::m_broad_phase = nullptr;
    btConstraintSolver* PhysicsEngine::m_solver = nullptr;

    std::vector<btCollisionObject*> PhysicsEngine::m_CollisionObjects;
    std::vector<btCollisionShape*> PhysicsEngine::m_CollisionShapes;
    // std::shared_ptr<Scene> PhysicsEngine::m_ActiveScene = nullptr;

    void PhysicsEngine::Init()
    {
        COFFEE_CORE_INFO("Initializing Physics Engine");

        m_collision_conf = new btDefaultCollisionConfiguration();
        m_dispatcher = new btCollisionDispatcher(m_collision_conf);
        m_broad_phase = new btDbvtBroadphase();
        m_solver = new btSequentialImpulseConstraintSolver();

        m_world = new btDiscreteDynamicsWorld(m_dispatcher, m_broad_phase, m_solver, m_collision_conf);

        SetGravity(GlobalGravity);
    }

    void PhysicsEngine::Update(float dt)
    {
        if (m_world)
        {
            m_world->stepSimulation(dt, 10);
            m_world->debugDrawWorld();

            // Debug print positions of all rigidbodies
            int numCollisionObjects = m_world->getNumCollisionObjects();
            for (int i = 0; i < numCollisionObjects; i++) 
            {
                btCollisionObject* obj = m_world->getCollisionObjectArray()[i];
                btRigidBody* body = btRigidBody::upcast(obj);
                
                if (body && body->getMotionState())
                {
                    btTransform trans;
                    body->getMotionState()->getWorldTransform(trans);
                    btVector3 pos = trans.getOrigin();
                    COFFEE_CORE_INFO("Rigidbody {0} position: ({1}, {2}, {3})", 
                        i, pos.x(), pos.y(), pos.z());
                }
            }
        }

      

        /*if (debugDrawList.empty())
            return;
        for (const auto& cmd : debugDrawList)
        {
            switch (cmd.type)
            {
            case CollisionShapeType::BOX:
                Coffee::DebugRenderer::DrawBox(cmd.position + glm::vec3(10,0,0), cmd.rotation,
                                               cmd.size, 
                                               cmd.color);
                break;

            case CollisionShapeType::SPHERE:
                Coffee::DebugRenderer::DrawSphere(cmd.position,
                                                  cmd.size.x, 
                                                  cmd.color);
                break;

            }
        }*/

    }

    void PhysicsEngine::ApplyRigidbody(RigidbodyComponent& rigidbodyComponent, TransformComponent& transformComponent,
                                       float dt)
    {
        if (!Scene::m_RigidbodyEntities.empty())
        {
            for (auto entity : Scene::m_RigidbodyEntities)
            {
                if (rigidbodyComponent.cfg.type == RigidBodyType::Static)
                    continue;

                rigidbodyComponent.m_RigidBody->GetConfig(rigidbodyComponent.cfg);

                if (rigidbodyComponent.cfg.type == RigidBodyType::Dynamic)
                {
                    if (rigidbodyComponent.cfg.UseGravity && !rigidbodyComponent.cfg.FreezeY)
                    {
                        glm::vec3 gravity = GetGravity();
                        // gravity *= 0.1f; 
                        
                        if (rigidbodyComponent.cfg.shapeConfig.mass > 0.0f)
                        {
                            rigidbodyComponent.cfg.Acceleration += gravity * dt;
                            rigidbodyComponent.ApplyDrag();
                        }
                    }

                    if (rigidbodyComponent.cfg.type == RigidBodyType::Kinematic)
                    {
                        transformComponent.Position += rigidbodyComponent.cfg.Velocity * dt;
                        return;
                    }

                    rigidbodyComponent.cfg.Velocity += rigidbodyComponent.cfg.Acceleration * dt;

                    if (!rigidbodyComponent.cfg.FreezeX)
                        transformComponent.Position.x += rigidbodyComponent.cfg.Velocity.x * dt;
                    if (!rigidbodyComponent.cfg.FreezeY)
                        transformComponent.Position.y += rigidbodyComponent.cfg.Velocity.y * dt;
                    if (!rigidbodyComponent.cfg.FreezeZ)
                        transformComponent.Position.z += rigidbodyComponent.cfg.Velocity.z * dt;
                    if (rigidbodyComponent.cfg.FreezeRotX)
                        rigidbodyComponent.cfg.Velocity.x = 0.0f;
                    if (rigidbodyComponent.cfg.FreezeRotY)
                        rigidbodyComponent.cfg.Velocity.y = 0.0f;
                    if (rigidbodyComponent.cfg.FreezeRotZ)
                        rigidbodyComponent.cfg.Velocity.z = 0.0f;

                    rigidbodyComponent.ApplyAngularDrag();
                }
            }
        }
    }
    void PhysicsEngine::Destroy()
    {
        for (auto* obj : m_CollisionObjects)
        {
            m_world->removeCollisionObject(obj);
            delete obj;
        }
        m_CollisionObjects.clear();

        for (auto* shape : m_CollisionShapes)
        {
            delete shape;
        }
        m_CollisionShapes.clear();

        delete m_world;
        delete m_solver;
        delete m_broad_phase;
        delete m_dispatcher;
        delete m_collision_conf;

        m_world = nullptr;
        m_solver = nullptr;
        m_broad_phase = nullptr;
        m_dispatcher = nullptr;
        m_collision_conf = nullptr;
    }

    void PhysicsEngine::SetGravity(const glm::vec3& gravity)
    {
        
        if (m_world)
        {
            m_world->setGravity(PhysUtils::GlmToBullet(gravity));
        }
    }

    glm::vec3 PhysicsEngine::GetGravity()
    {
        if (m_world)
        {
            return PhysUtils::BulletToGlm(m_world->getGravity());
        }
        return glm::vec3(0.0f);
    }

    // void PhysicsEngine::ProcessTriggerEvents()
    //{
    //     if (m_world == nullptr) return;

    //    // TODO cuando haya colliders descomentar esto y adaptarlo correctamente
    //    /*
    //    int numManifolds = m_world->getDispatcher()->getNumManifolds();
    //    for(int i = 0; i<numManifolds; i++)
    //    {
    //        btPersistentManifold* contactManifold = m_world->getDispatcher()->getManifoldByIndexInternal(i);
    //        auto* obA = (btCollisionObject*)(contactManifold->getBody0());
    //        auto* obB = (btCollisionObject*)(contactManifold->getBody1());

    //        int numContacts = contactManifold->getNumContacts();
    //        if(numContacts > 0)
    //        {
    //            Collider* body_a = (Collider*)obA->getUserPointer();
    //            Collider* body_b = (Collider*)obB->getUserPointer();

    //            if(body_a && body_b)
    //            {
    //                std::vector<CollisionCallback> items = body_a->collision_listeners;
    //                for (auto& item : items)
    //                {
    //                    item(body_a, body_b);
    //                }
    //                items = body_b->collision_listeners;
    //                for (auto& item : items)
    //                {
    //                    item(body_b, body_a);
    //                }
    //            }
    //        }
    //    }
    //    */
    //}

    btCollisionShape* PhysicsEngine::CreateCollisionShape(const CollisionShapeConfig& config)
    {
        btCollisionShape* shape = nullptr;

        switch (config.type)
        {
        case CollisionShapeType::BOX:
            shape = new btBoxShape(PhysUtils::GlmToBullet(config.size * 0.5f));
            break;
        case CollisionShapeType::SPHERE:
            shape = new btSphereShape(config.size.x);
            break;
        case CollisionShapeType::CAPSULE:
            shape = new btCapsuleShape(config.size.x, config.size.y);
            break;
        case CollisionShapeType::CYLINDER:
            shape = new btCylinderShape(PhysUtils::GlmToBullet(config.size));
            break;
        default:
            shape = new btBoxShape(PhysUtils::GlmToBullet(config.size * 0.5f));
            break;
        }

        m_CollisionShapes.push_back(shape);
        return shape;
    }

    void PhysicsEngine::AddDebugDrawCommand(CollisionShapeType type, const glm::vec3& position,
                                            const glm::quat& rotation, const glm::vec3& size, const glm::vec4& color)
    {
        debugDrawList.push_back({position, rotation, size, color, type});
    }

    btCollisionObject* PhysicsEngine::CreateCollisionObject(const CollisionShapeConfig& config,
                                                            const glm::vec3& position, const glm::vec3& scale,
                                                            const glm::quat& rotation)
    {
        // Adjust the size based on the GameObject's scale
        glm::vec3 adjustedSize = config.size;

        // Create the collision shape with the adjusted size
        btCollisionShape* shape = nullptr;

        switch (config.type)
        {
        case CollisionShapeType::BOX:
            shape = new btBoxShape(PhysUtils::GlmToBullet(adjustedSize * 0.5f)); // Half extents
            break;
        case CollisionShapeType::SPHERE:
            shape = new btSphereShape(config.size.x);
            break;
        // Other types...
        default:
            shape = new btBoxShape(PhysUtils::GlmToBullet(adjustedSize * 0.5f));
            break;
        }

        btCollisionObject* object = nullptr;

        if (config.isTrigger)
        {
            // Create a simple collision object for triggers
            object = new btCollisionObject();
            object->setCollisionShape(shape);
            object->setCollisionFlags(object->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }
        else
        {
            btVector3 localInertia(0, 0, 0);

            // Calculate local inertia if the object has mass
            if (config.mass != 0.0f)
            {
                shape->calculateLocalInertia(config.mass, localInertia);
            }

            // Create the motion state with initial transform
            btDefaultMotionState* motionState = new btDefaultMotionState(
                btTransform(PhysUtils::GlmToBullet(rotation), PhysUtils::GlmToBullet(position)));

            // Set up rigid body construction info
            btRigidBody::btRigidBodyConstructionInfo rbInfo(config.mass, motionState, shape, localInertia);

            // Create the rigid body
            btRigidBody* rigidBody = new btRigidBody(rbInfo);

            object = rigidBody;
        }

        // Set the shape and initial transformation
        object->setCollisionShape(shape);
        btTransform transform;
        transform.setIdentity();
        transform.setOrigin(PhysUtils::GlmToBullet(position));
        transform.setRotation(PhysUtils::GlmToBullet(rotation));
        object->setWorldTransform(transform);

        if (object->getInternalType() == btCollisionObject::CO_RIGID_BODY)
        {
            btRigidBody* body = static_cast<btRigidBody*>(object);
            m_world->addRigidBody(body);
        }
        else
        {
            m_world->addCollisionObject(object);
        }

        m_CollisionObjects.push_back(object);

        return object;
    }

    void PhysicsEngine::DestroyCollisionObject(btCollisionObject* object)
    {
        if (!object)
            return;

        m_world->removeCollisionObject(object);

        auto it = std::find(m_CollisionObjects.begin(), m_CollisionObjects.end(), object);
        if (it != m_CollisionObjects.end())
        {
            m_CollisionObjects.erase(it);
        }

        // if rigid body, delete motion state
        btRigidBody* body = btRigidBody::upcast(object);
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
        }

        delete object;
    }

    void PhysicsEngine::ProcessTriggerEvents()
    {
        if (!m_world)
            return;

        int numManifolds = m_world->getDispatcher()->getNumManifolds();
        for (int i = 0; i < numManifolds; ++i)
        {
            btPersistentManifold* contactManifold = m_world->getDispatcher()->getManifoldByIndexInternal(i);

            const btCollisionObject* objA = contactManifold->getBody0();
            const btCollisionObject* objB = contactManifold->getBody1();

            if (contactManifold->getNumContacts() > 0)
            {
                // Aseg�rate de que los UserPointers son v�lidos y convertibles
                if (objA->getUserPointer() && objB->getUserPointer())
                {
                    Collider* colliderA = dynamic_cast<Collider*>(static_cast<Collider*>(objA->getUserPointer()));
                    Collider* colliderB = dynamic_cast<Collider*>(static_cast<Collider*>(objB->getUserPointer()));

                    if (colliderA && colliderB)
                    {
                        colliderA->OnCollision(colliderB);
                        colliderB->OnCollision(colliderA);
                    }
                }
            }
        }
    }

    void PhysicsEngine::SetPosition(btCollisionObject* object, const glm::vec3& position)
    {
        if (!object)
            return;

        btTransform transform = object->getWorldTransform();
        transform.setOrigin(PhysUtils::GlmToBullet(position));
        object->setWorldTransform(transform);
    }

    glm::vec3 PhysicsEngine::GetPosition(btCollisionObject* object)
    {
        if (!object)
            return glm::vec3(0.0f);

        const btTransform& transform = object->getWorldTransform();
        return PhysUtils::BulletToGlm(transform.getOrigin());
    }

    int PhysicsEngine::GetRigidbodyFlags(const RigidBodyConfig& config)
    {
        int flags = 0;
        switch (config.type)
        {
        case RigidBodyType::Static:
            flags |= btCollisionObject::CF_STATIC_OBJECT;
            break;
        case RigidBodyType::Kinematic:
            flags |= btCollisionObject::CF_KINEMATIC_OBJECT;
            break;
        case RigidBodyType::Dynamic:
            flags |= btCollisionObject::CF_DYNAMIC_OBJECT;
            break;
        }
        return flags;
    }
    btRigidBody* PhysicsEngine::CreateRigidBody(CollisionCallbacks* colCallbacks, const RigidBodyConfig& config)
    {
        auto shape = CreateCollisionShape(config.shapeConfig);

        btVector3 localInertia(0, 0, 0);
        if (config.type != RigidBodyType::Static)
            shape->calculateLocalInertia(config.shapeConfig.mass, localInertia);

        btDefaultMotionState* motionState =
            new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));

        btRigidBody::btRigidBodyConstructionInfo rbInfo(config.shapeConfig.mass, motionState, shape, localInertia);
        
        rbInfo.m_linearDamping = config.LinearDrag;
        rbInfo.m_angularDamping = config.AngularDrag;

        btRigidBody* body = new btRigidBody(rbInfo);
        
        // Set object type
        body->setFlags(body->getFlags() | GetRigidbodyFlags(config));

        body->setUserPointer(colCallbacks);
        m_world->addRigidBody(body);

        return body;
    }
    void PhysicsEngine::RemoveRigidBody(btRigidBody* rigidBody)
    {
        if (m_world)
            m_world->removeRigidBody(rigidBody);
    }

} // namespace Coffee