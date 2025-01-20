#include "PhysicsEngine.h"

namespace Coffee {

    btDynamicsWorld* PhysicsEngine::dynamicsWorld = nullptr;
    btCollisionConfiguration* PhysicsEngine::collision_conf = nullptr;
    btDispatcher* PhysicsEngine::dispatcher = nullptr;
    btBroadphaseInterface* PhysicsEngine::broad_phase = nullptr;
    btConstraintSolver* PhysicsEngine::solver = nullptr;
    btVehicleRaycaster* PhysicsEngine::vehicle_raycaster = nullptr;
    std::vector<btCollisionObject*> PhysicsEngine::m_CollisionObjects;
    std::vector<btCollisionShape*> PhysicsEngine::m_CollisionShapes;

    void PhysicsEngine::Init()
    {
        collision_conf = new btDefaultCollisionConfiguration();
        dispatcher = new btCollisionDispatcher(collision_conf);
        broad_phase = new btDbvtBroadphase();
        solver = new btSequentialImpulseConstraintSolver();

        dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broad_phase, solver, collision_conf);
        
        SetGravity(glm::vec3(0.0f, -9.81f, 0.0f));
    }

    void PhysicsEngine::Update(float dt)
    {
        if (dynamicsWorld)
        {
            dynamicsWorld->stepSimulation(dt, 10);
        }
    }

    void PhysicsEngine::Destroy()
    {
        for (auto* obj : m_CollisionObjects)
        {
            dynamicsWorld->removeCollisionObject(obj);
            delete obj;
        }
        m_CollisionObjects.clear();

        for (auto* shape : m_CollisionShapes)
        {
            delete shape;
        }
        m_CollisionShapes.clear();

        delete dynamicsWorld;
        delete solver;
        delete broad_phase;
        delete dispatcher;
        delete collision_conf;
        delete vehicle_raycaster;

        dynamicsWorld = nullptr;
        solver = nullptr;
        broad_phase = nullptr;
        dispatcher = nullptr;
        collision_conf = nullptr;
        vehicle_raycaster = nullptr;
    }

    void PhysicsEngine::SetGravity(const glm::vec3& gravity)
    {
        if (dynamicsWorld)
        {
            dynamicsWorld->setGravity(GlmToBullet(gravity));
        }
    }

    glm::vec3 PhysicsEngine::GetGravity()
    {
        if (dynamicsWorld)
        {
            return BulletToGlm(dynamicsWorld->getGravity());
        }
        return glm::vec3(0.0f);
    }

    btVector3 PhysicsEngine::GlmToBullet(const glm::vec3& v)
    {
        return btVector3(v.x, v.y, v.z);
    }

    glm::vec3 PhysicsEngine::BulletToGlm(const btVector3& v)
    {
        return glm::vec3(v.x(), v.y(), v.z());
    }

    btCollisionShape* PhysicsEngine::CreateCollisionShape(const CollisionShapeConfig& config)
    {
        btCollisionShape* shape = nullptr;
        
        switch (config.type)
        {
            case CollisionShapeType::BOX:
                shape = new btBoxShape(GlmToBullet(config.size * 0.5f));
                break;
            case CollisionShapeType::SPHERE:
                shape = new btSphereShape(config.size.x);
                break;
            case CollisionShapeType::CAPSULE:
                shape = new btCapsuleShape(config.size.x, config.size.y);
                break;
            case CollisionShapeType::CYLINDER:
                shape = new btCylinderShape(GlmToBullet(config.size));
                break;
            default:
                shape = new btBoxShape(GlmToBullet(config.size * 0.5f));
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
                btTransform(btQuaternion(0, 0, 0, 1), GlmToBullet(position))
            );

            btRigidBody::btRigidBodyConstructionInfo rbInfo(
                config.mass, motionState, shape, localInertia
            );

            object = new btRigidBody(rbInfo);
        }

        object->setCollisionShape(shape);
        object->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1), GlmToBullet(position)));

        if (config.isTrigger)
        {
            object->setCollisionFlags(object->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }

        dynamicsWorld->addCollisionObject(object);
        m_CollisionObjects.push_back(object);

        return object;
    }

    void PhysicsEngine::DestroyCollisionObject(btCollisionObject* object)
    {
        if (!object) return;

        dynamicsWorld->removeCollisionObject(object);
        
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
}