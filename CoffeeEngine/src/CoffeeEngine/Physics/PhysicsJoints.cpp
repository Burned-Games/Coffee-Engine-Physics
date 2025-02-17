#include "CoffeeEngine/Physics/PhysicsJoints.h"
#include "CoffeeEngine/Core/Log.h"

namespace Coffee
{

    std::unordered_map<std::string, std::unique_ptr<btTypedConstraint>> PhysicsJoints::joints_;

    void PhysicsJoints::Init()
    {
        COFFEE_CORE_INFO("Initializing Physics Joints");
    }

    btTypedConstraint* PhysicsJoints::createJoint(const std::string& id, const JointConfig& config)
    {
        if (!config.bodyA || !config.bodyB)
        {
            throw std::runtime_error("Invalid rigid body pointers provided.");
        }

        if (joints_.find(id) != joints_.end())
        {
            throw std::runtime_error("Joint with id '" + id + "' already exists.");
        }

        btTypedConstraint* joint = nullptr;

        switch (config.type)
        {
        case JointType::POINT2POINT:
            joint = new btPoint2PointConstraint(*config.bodyA, *config.bodyB, config.pivotInA, config.pivotInB);
            break;
        case JointType::HINGE:
            joint = new btHingeConstraint(*config.bodyA, *config.bodyB, config.pivotInA, config.pivotInB,
                                          config.axisInA, config.axisInB);
            break;
        case JointType::SLIDER:
            joint = new btSliderConstraint(*config.bodyA, *config.bodyB, btTransform::getIdentity(),
                                           btTransform::getIdentity(), config.useLinearReferenceFrameA);
            break;
        case JointType::CONETWIST:
            joint = new btConeTwistConstraint(*config.bodyA, *config.bodyB, btTransform::getIdentity(),
                                              btTransform::getIdentity());
            break;
        case JointType::GENERIC6DOF:
            joint = new btGeneric6DofConstraint(*config.bodyA, *config.bodyB, btTransform::getIdentity(),
                                                btTransform::getIdentity(), config.useLinearReferenceFrameA);
            break;
        default:
            throw std::runtime_error("Unsupported JointType.");
        }

        joints_[id] = std::unique_ptr<btTypedConstraint>(joint);
        return joint;
    }

    void PhysicsJoints::removeJoint(const std::string& id)
    {
        auto it = joints_.find(id);
        if (it == joints_.end())
        {
            throw std::runtime_error("Joint with id '" + id + "' does not exist.");
        }
        joints_.erase(it);
    }

    btTypedConstraint* PhysicsJoints::getJoint(const std::string& id)
    {
        auto it = joints_.find(id);
        return (it != joints_.end()) ? it->second.get() : nullptr;
    }

    void PhysicsJoints::addToWorld(btDynamicsWorld* world)
    {
        for (auto& [id, joint] : joints_)
        {
            world->addConstraint(joint.get(), true);
        }
    }

    void PhysicsJoints::removeFromWorld(btDynamicsWorld* world)
    {
        for (auto& [id, joint] : joints_)
        {
            world->removeConstraint(joint.get());
        }
    }

    void PhysicsJoints::Destroy()
    {
        deleteAllJoints();
    }

    void PhysicsJoints::deleteAllJoints()
    {
        joints_.clear();
    }

} // namespace Coffee
