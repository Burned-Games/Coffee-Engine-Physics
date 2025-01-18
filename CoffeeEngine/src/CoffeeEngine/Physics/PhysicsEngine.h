#pragma once
#include <bullet/btBulletDynamicsCommon.h>

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
