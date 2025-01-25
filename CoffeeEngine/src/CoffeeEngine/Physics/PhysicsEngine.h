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

    // collision shape types
    enum class CollisionShapeType {
        BOX,
        SPHERE,
        CAPSULE,
        CYLINDER,
        MESH
    };

    struct CollisionShapeConfig {
        CollisionShapeType type = CollisionShapeType::BOX;
        glm::vec3 size = glm::vec3(1.0f);  // size of the shape
        bool isTrigger = false;
        float mass = 1.0f;
        bool isStatic = false;
    };

    struct RigidBodyConfig
    {
        CollisionShapeConfig shapeConfig;
        bool IsStatic = false;                       ///< Whether the object is static (non-moving) or dynamic (moving).
        bool IsKinematic = false;                    ///< Whether the object is static (non-moving) or dynamic (moving).
        bool UseGravity = true;                      ///< Whether the object is affected by gravity.
        float Mass = 1.0f;                           ///< The mass of the rigidbody. 0 means the rigidbody is kinematic
        glm::vec3 Velocity = {0.0f, 0.0f, 0.0f};     ///< The current velocity of the rigidbody.
        glm::vec3 Acceleration = {0.0f, 0.0f, 0.0f}; ///< The current acceleration of the rigidbody.

        float LinearDrag = 0.1f; ///< The linear drag of the rigidbody.
        float AngularDrag = 0.1f;

        bool FreezeX = false;
        bool FreezeY = false;
        bool FreezeZ = false;
        bool FreezeRotationX = false;
        bool FreezeRotationY = false;
        bool FreezeRotationZ = false;
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

    };
}
