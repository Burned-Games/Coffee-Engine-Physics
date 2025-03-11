#pragma once

#define BT_NO_SIMD_OPERATOR_OVERLOADS

#include <btBulletDynamicsCommon.h>
#include <vector>

namespace Coffee {

    constexpr float GRAVITY = -9.81f;

    class PhysicsWorld {

    public:
        PhysicsWorld();
        ~PhysicsWorld();

        void addRigidBody(btRigidBody* body) const;
        void removeRigidBody(btRigidBody* body) const;
        void addCollisionObject(btCollisionObject* bt_collision_object) const;
        void removeCollisionObject(btCollisionObject* bt_collision_object) const;
        void stepSimulation(float dt) const;

        void setGravity(float gravity) const;
        void setGravity(const btVector3& gravity) const;

        btDiscreteDynamicsWorld* getDynamicsWorld() const;

        void drawCollisionShapes() const;


      private:
        btDefaultCollisionConfiguration* collisionConfig;
        btCollisionDispatcher* dispatcher;
        btBroadphaseInterface* broadphase;
        btSequentialImpulseConstraintSolver* solver;
        btDiscreteDynamicsWorld* dynamicsWorld;
    };

}