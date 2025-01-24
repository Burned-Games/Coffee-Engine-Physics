#include "PhysicsEngine.h"
#include "PhysUtils.h"


#include "CoffeeEngine/Core/Log.h"
#include "Vehicle.h"
#include "CoffeeEngine/Scene/Components.h"

#include <entt/entity/entity.hpp>

namespace Coffee {

    entt::registry PhysicsEngine::m_EntityRegistry; 
    using namespace Coffee;
    
    btDynamicsWorld* PhysicsEngine::m_world = nullptr;
    btCollisionConfiguration* PhysicsEngine::m_collision_conf = nullptr;
    btDispatcher* PhysicsEngine::m_dispatcher = nullptr;
    btBroadphaseInterface* PhysicsEngine::m_broad_phase = nullptr;
    btConstraintSolver* PhysicsEngine::m_solver = nullptr;
    btVehicleRaycaster* PhysicsEngine::m_vehicle_raycaster = nullptr;
    DebugDrawer* PhysicsEngine::m_debug_draw = nullptr;
    
    std::vector<btCollisionObject*> PhysicsEngine::m_CollisionObjects;
    std::vector<btCollisionShape*> PhysicsEngine::m_CollisionShapes;
    Vehicle vehicle;
    //std::shared_ptr<Scene> PhysicsEngine::m_ActiveScene = nullptr;

    void PhysicsEngine::Init()
    {
        COFFEE_CORE_INFO("Initializing Physics Engine");
        
        m_collision_conf = new btDefaultCollisionConfiguration();
        m_dispatcher = new btCollisionDispatcher(m_collision_conf);
        m_broad_phase = new btDbvtBroadphase();
        m_solver = new btSequentialImpulseConstraintSolver();

        m_world = new btDiscreteDynamicsWorld(m_dispatcher, m_broad_phase, m_solver, m_collision_conf);

        m_debug_draw = new DebugDrawer();

          

        m_world->setDebugDrawer(m_debug_draw);

        SetGravity(glm::vec3(0.0f, -9.81f, 0.0f));
    }

   void PhysicsEngine::Update(float dt)
    {
        if (m_world)
        {
            m_world->stepSimulation(dt, 10);

            if (!Scene::m_RigidbodyEntities.empty())
            {
                for (auto entity : Scene::m_RigidbodyEntities)
                {
                    COFFEE_CORE_INFO("Entities with RigidbodyComponent found - phyisicsEngine.");

                }
            }
            auto view = m_EntityRegistry.view<RigidbodyComponent>();
           /* if (view.empty())
            {
                COFFEE_CORE_INFO("No entities with RigidbodyComponent found.");
            }
            else
            {
                COFFEE_CORE_INFO("Entities with RigidbodyComponent found.");
            }*/

            for (auto entity : view)
            {
                auto& rigidbody = view.get<RigidbodyComponent>(entity);
                auto& transform = m_EntityRegistry.get<TransformComponent>(entity);

                if (rigidbody.UseGravity && !rigidbody.IsStatic && !rigidbody.FreezeY)
                {
                    glm::vec3 gravity = GetGravity(); //(0.0f, -9.81f, 0.0f);
                    if (rigidbody.Mass > 0.0f)
                    {
                        glm::vec3 gravityAcceleration = gravity / rigidbody.Mass;
                        rigidbody.Acceleration += gravityAcceleration;
                        rigidbody.Velocity += rigidbody.Acceleration * dt;
                        transform.Position += rigidbody.Velocity * dt; 
                    }
                }

                // Aplicar restricciones de movimiento
                if (!rigidbody.FreezeX)
                    transform.Position.x += rigidbody.Velocity.x * dt;
                if (!rigidbody.FreezeY)
                    transform.Position.y += rigidbody.Velocity.y * dt;
                if (!rigidbody.FreezeZ)
                    transform.Position.z += rigidbody.Velocity.z * dt;

                // Si el objeto está congelado en alguna rotación, establecemos su velocidad en 0
                if (rigidbody.FreezeRotationX)
                    rigidbody.Velocity.x = 0.0f;
                if (rigidbody.FreezeRotationY)
                    rigidbody.Velocity.y = 0.0f;
                if (rigidbody.FreezeRotationZ)
                    rigidbody.Velocity.z = 0.0f;

                // Si el cuerpo es estático, no se mueve ni se le aplica aceleración
                if (rigidbody.IsStatic)
                {
                    rigidbody.Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
                }
            }
        }

        //vehicle.update(dt);
    }

   void PhysicsEngine::ApplyRigidbody(RigidbodyComponent& rigidbodyComponent, TransformComponent& transfromComponent)
    { 
       if (!Scene::m_RigidbodyEntities.empty())
       {
           for (auto entity : Scene::m_RigidbodyEntities)
           {
               COFFEE_CORE_INFO("Entities with RigidbodyComponent found - phyisicsEngine - applyrigidbody");

               if (rigidbodyComponent.UseGravity && !rigidbodyComponent.IsStatic && !rigidbodyComponent.FreezeY) 
               {
                   glm::vec3 gravity = GetGravity(); //(0.0f, -9.81f, 0.0f);
                   if (rigidbodyComponent.Mass > 0.0f)
                   {
                       //falta aplicar el dt...
                       glm::vec3 gravityAcceleration = gravity / rigidbodyComponent.Mass; 
                       rigidbodyComponent.Acceleration += gravityAcceleration; 
                       rigidbodyComponent.Velocity += rigidbodyComponent.Acceleration; 
                       transfromComponent.Position += rigidbodyComponent.Velocity;  
                   }
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
        delete m_vehicle_raycaster;

        m_world = nullptr;
        m_solver = nullptr;
        m_broad_phase = nullptr;
        m_dispatcher = nullptr;
        m_collision_conf = nullptr;
        m_vehicle_raycaster = nullptr;
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

    //void PhysicsEngine::ProcessTriggerEvents()
    //{
    //    if (m_world == nullptr) return;

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

    btCollisionObject* PhysicsEngine::CreateCollisionObject(const CollisionShapeConfig& config, const glm::vec3& position)
    {
        btCollisionShape* shape = CreateCollisionShape(config);
        
        btCollisionObject* object = nullptr;
        
        if (config.isStatic || config.isTrigger)
        {
            object = new btCollisionObject();
        }
        else
        {
            btVector3 localInertia(0, 0, 0);
            if (!config.isStatic)
                shape->calculateLocalInertia(config.mass, localInertia);

            btDefaultMotionState* motionState = new btDefaultMotionState(
                btTransform(btQuaternion(0, 0, 0, 1), PhysUtils::GlmToBullet(position))
            );

            btRigidBody::btRigidBodyConstructionInfo rbInfo(
                config.mass, motionState, shape, localInertia
            );

            object = new btRigidBody(rbInfo);
        }

        object->setCollisionShape(shape);
        object->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1), PhysUtils::GlmToBullet(position)));

        if (config.isTrigger)
        {
            object->setCollisionFlags(object->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }

        m_world->addCollisionObject(object);
        m_CollisionObjects.push_back(object);

        return object;
    }

    void PhysicsEngine::DestroyCollisionObject(btCollisionObject* object)
    {
        if (!object) return;

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
                // Asegúrate de que los UserPointers son válidos y convertibles
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



}