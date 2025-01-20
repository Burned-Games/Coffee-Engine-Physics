#include "PhysicsEngine.h"
#include "PhysUtils.h"

#include "CoffeeEngine/Core/Log.h"

namespace Coffee {

    using namespace Coffee;
    
    btDynamicsWorld* PhysicsEngine::dynamicsWorld = nullptr;
    btCollisionConfiguration* PhysicsEngine::collision_conf = nullptr;
    btDispatcher* PhysicsEngine::dispatcher = nullptr;
    btBroadphaseInterface* PhysicsEngine::broad_phase = nullptr;
    btConstraintSolver* PhysicsEngine::solver = nullptr;
    btVehicleRaycaster* PhysicsEngine::vehicle_raycaster = nullptr;

    void PhysicsEngine::Init()
    {
        COFFEE_CORE_INFO("Initializing Physics Engine");
        
        collision_conf = new btDefaultCollisionConfiguration();
        dispatcher = new btCollisionDispatcher(collision_conf);
        broad_phase = new btDbvtBroadphase();
        solver = new btSequentialImpulseConstraintSolver();

        dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broad_phase, solver, collision_conf);

        debug_draw = new DebugDrawer();

        dynamicsWorld->setDebugDrawer(debug_draw);

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
            dynamicsWorld->setGravity(PhysUtils::GlmToBullet(gravity));
        }
    }

    glm::vec3 PhysicsEngine::GetGravity()
    {
        if (dynamicsWorld)
        {
            return PhysUtils::BulletToGlm(dynamicsWorld->getGravity());
        }
        return glm::vec3(0.0f);
    }
    
    void PhysicsEngine::ProcessCollisionEvents()
    {
        if (!dynamicsWorld) return;    
    }

    void PhysicsEngine::ProcessTriggerEvents()
    {
        
    }
}