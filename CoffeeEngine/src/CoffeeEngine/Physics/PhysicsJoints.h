#pragma once

#include <bullet/btBulletDynamicsCommon.h>
#include <memory>
#include <string>
#include <unordered_map>

namespace Coffee
{

    enum class JointType
    {
        POINT2POINT,
        HINGE,
        SLIDER,
        CONETWIST,
        GENERIC6DOF
    };

    struct JointConfig
    {
        JointType type;
        btRigidBody* bodyA = nullptr;
        btRigidBody* bodyB = nullptr;
        btVector3 pivotInA = {0, 0, 0};
        btVector3 pivotInB = {0, 0, 0};
        btVector3 axisInA = {0, 1, 0};
        btVector3 axisInB = {0, 1, 0};
        bool useLinearReferenceFrameA = true;
    };

    class PhysicsJoints
    {
      public:
        static void Init();
        static btTypedConstraint* createJoint(const std::string& id, const JointConfig& config);
        static void removeJoint(const std::string& id);
        static btTypedConstraint* getJoint(const std::string& id);

        static void addToWorld(btDynamicsWorld* world);
        static void removeFromWorld(btDynamicsWorld* world);

        static void Destroy();

      private:
        static std::unordered_map<std::string, std::unique_ptr<btTypedConstraint>> joints_;
        static void deleteAllJoints();
    };

} // namespace Coffee
