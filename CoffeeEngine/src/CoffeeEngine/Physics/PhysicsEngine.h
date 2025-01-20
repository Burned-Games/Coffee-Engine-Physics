#pragma once
#include <bullet/btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include <vector>

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

    static class PhysicsEngine {
    public:
        static void Init();
        static void Update(float dt);
        static void Destroy();

        static btDynamicsWorld* GetWorld() { return dynamicsWorld; }

        static void SetGravity(const glm::vec3& gravity);
        static glm::vec3 GetGravity();

        static btVector3 GlmToBullet(const glm::vec3& v);
        static glm::vec3 BulletToGlm(const btVector3& v);

        // Create + Destroy collision objects
        static btCollisionObject* CreateCollisionObject(const CollisionShapeConfig& config, const glm::vec3& position);
        static void DestroyCollisionObject(btCollisionObject* object);
        
        static btCollisionShape* CreateCollisionShape(const CollisionShapeConfig& config);

    private:

        static btDynamicsWorld* dynamicsWorld;

        static btCollisionConfiguration*	collision_conf;
        static btDispatcher*				dispatcher;
        static btBroadphaseInterface*		broad_phase;
        static btConstraintSolver*          solver;
        static btVehicleRaycaster*			vehicle_raycaster;
        //static DebugDrawer*				debug_draw;

        static std::vector<btCollisionObject*> m_CollisionObjects;
        static std::vector<btCollisionShape*> m_CollisionShapes;
    };

}
