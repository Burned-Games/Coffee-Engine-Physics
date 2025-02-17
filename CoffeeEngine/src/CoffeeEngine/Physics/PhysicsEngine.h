#pragma once
#include "DebugDrawer.h"

#include <bullet/btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include <vector>
#include "Collider.h"
#include "CoffeeEngine/Scene/Entity.h"
#include "CoffeeEngine/Scene/Components.h"
#include "CollisionCallbacks.h"
#include <entt/entt.hpp>

namespace Coffee{

    enum class PhysicsType
    {
        BASIC,
        DISCRETE,
        PARALLEL,
        CONTINUOUS
    };

    

    static class PhysicsEngine
    {
    public:

        static entt::registry m_EntityRegistry;

        static void Init();
        static void Update(float dt);
        static void Destroy();
        static void ApplyRigidbody(RigidbodyComponent& rigidbodyComponent, TransformComponent& transformComponent, float dt);

        static btDynamicsWorld* GetWorld() { return m_world; }

        static void SetGravity(const glm::vec3& gravity);
        static glm::vec3 GetGravity();

        // Call collision delegates for triggers
        static void ProcessTriggerEvents();

        static void SetPosition(btCollisionObject* object, const glm::vec3& position);
        static int GetRigidbodyFlags(const RigidBodyConfig& config);

        // Create + Destroy collision objects
        static btCollisionObject* CreateCollisionObject(const CollisionShapeConfig& config, const glm::vec3& position);
        static void DestroyCollisionObject(btCollisionObject* object);
        
        static btCollisionShape* CreateCollisionShape(const CollisionShapeConfig& config);

        static btRigidBody* CreateRigidBody(CollisionCallbacks* colCallbacks, const RigidBodyConfig& config);
        static void RemoveRigidBody(btRigidBody* rigidBody);

    private:

        static btDynamicsWorld* m_world;

        static btCollisionConfiguration*	m_collision_conf;
        static btDispatcher*				m_dispatcher;
        static btBroadphaseInterface*		m_broad_phase;
        static btConstraintSolver*          m_solver;
        static btVehicleRaycaster*			m_vehicle_raycaster;
        static DebugDrawer*				    m_debug_draw;

        static std::vector<btCollisionObject*> m_CollisionObjects;
        static std::vector<btCollisionShape*> m_CollisionShapes;

        static std::vector<RigidBody*> m_Rigidbodies;
        friend class RigidBody;
    };
}
