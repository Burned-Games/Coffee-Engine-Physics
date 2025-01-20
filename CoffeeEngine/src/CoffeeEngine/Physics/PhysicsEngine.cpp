#include "PhysicsEngine.h"
#include "PhysUtils.h"

#include "CoffeeEngine/Core/Log.h"

namespace Coffee {

    using namespace Coffee;
    
    btDynamicsWorld* PhysicsEngine::m_World = nullptr;
    btCollisionConfiguration* PhysicsEngine::m_collision_conf = nullptr;
    btDispatcher* PhysicsEngine::m_dispatcher = nullptr;
    btBroadphaseInterface* PhysicsEngine::m_broad_phase = nullptr;
    btConstraintSolver* PhysicsEngine::m_solver = nullptr;
    btVehicleRaycaster* PhysicsEngine::m_vehicle_raycaster = nullptr;
    std::vector<btCollisionObject*> PhysicsEngine::m_CollisionObjects;
    std::vector<btCollisionShape*> PhysicsEngine::m_CollisionShapes;

    void PhysicsEngine::Init()
    {
        COFFEE_CORE_INFO("Initializing Physics Engine");
        
        m_collision_conf = new btDefaultCollisionConfiguration();
        m_dispatcher = new btCollisionDispatcher(m_collision_conf);
        m_broad_phase = new btDbvtBroadphase();
        m_solver = new btSequentialImpulseConstraintSolver();

        m_World = new btDiscreteDynamicsWorld(m_dispatcher, m_broad_phase, m_solver, m_collision_conf);

        m_debug_draw = new DebugDrawer();

        m_World->setDebugDrawer(m_debug_draw);

        SetGravity(glm::vec3(0.0f, -9.81f, 0.0f));
    }

    void PhysicsEngine::Update(float dt)
    {
        if (m_World)
        {
            m_World->stepSimulation(dt, 10);
        }
    }

    void PhysicsEngine::Destroy()
    {
        for (auto* obj : m_CollisionObjects)
        {
            m_World->removeCollisionObject(obj);
            delete obj;
        }
        m_CollisionObjects.clear();

        for (auto* shape : m_CollisionShapes)
        {
            delete shape;
        }
        m_CollisionShapes.clear();

        delete m_World;
        delete m_solver;
        delete m_broad_phase;
        delete m_dispatcher;
        delete m_collision_conf;
        delete m_vehicle_raycaster;

        m_World = nullptr;
        m_solver = nullptr;
        m_broad_phase = nullptr;
        m_dispatcher = nullptr;
        m_collision_conf = nullptr;
        m_vehicle_raycaster = nullptr;
    }

    void PhysicsEngine::SetGravity(const glm::vec3& gravity)
    {
        if (m_World)
        {
            m_World->setGravity(PhysUtils::GlmToBullet(gravity));
        }
    }

    glm::vec3 PhysicsEngine::GetGravity()
    {
        if (m_World)
        {
            return PhysUtils::BulletToGlm(m_World->getGravity());
        }
        return glm::vec3(0.0f);
    }
    
    void PhysicsEngine::ProcessCollisionEvents()
    {
        if (!m_World) return;    
    }

    void PhysicsEngine::ProcessTriggerEvents()
    {
        
    }

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

        m_World->addCollisionObject(object);
        m_CollisionObjects.push_back(object);

        return object;
    }

    void PhysicsEngine::DestroyCollisionObject(btCollisionObject* object)
    {
        if (!object) return;

        m_World->removeCollisionObject(object);
        
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