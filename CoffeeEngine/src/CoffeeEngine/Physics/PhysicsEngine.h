#pragma once
#include <bullet/btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

namespace Coffee{


    enum class PhysicsType
    {
        BASIC,
        DISCRETE,
        PARALLEL,
        CONTINUOUS
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

    private:

        static btDynamicsWorld* dynamicsWorld;


        static btCollisionConfiguration*	collision_conf;
        static btDispatcher*				dispatcher;
        static btBroadphaseInterface*		broad_phase;
        static btConstraintSolver*          solver;
        static btVehicleRaycaster*			vehicle_raycaster;
        //static DebugDrawer*				debug_draw;
    };

}
