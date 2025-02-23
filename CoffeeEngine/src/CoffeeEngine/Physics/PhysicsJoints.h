#pragma once

/**
 * @file PhysicsJoints.h
 * @brief Defines the PhysicsJoints class for managing physics constraints.
 */

#include <bullet/btBulletDynamicsCommon.h>
#include <memory>
#include <string>
#include <unordered_map>

namespace Coffee
{

    /**
     * @enum JointType
     * @brief Types of joints that can be created.
     */
    enum class JointType
    {
        POINT2POINT, /**< Point-to-point constraint. */
        HINGE,       /**< Hinge constraint. */
        SLIDER,      /**< Slider constraint. */
        CONETWIST,   /**< Cone twist constraint. */
        GENERIC6DOF  /**< Generic 6 degrees of freedom constraint. */
    };

    /**
     * @struct JointConfig
     * @brief Configuration structure for defining a physics joint.
     */
    struct JointConfig
    {
        JointType type;                       /**< Type of joint. */
        btRigidBody* bodyA = nullptr;         /**< First rigid body involved in the joint. */
        btRigidBody* bodyB = nullptr;         /**< Second rigid body involved in the joint. */
        btVector3 pivotInA = {0, 0, 0};       /**< Pivot point in local space of body A. */
        btVector3 pivotInB = {0, 0, 0};       /**< Pivot point in local space of body B. */
        btVector3 axisInA = {0, 1, 0};        /**< Rotation axis in local space of body A. */
        btVector3 axisInB = {0, 1, 0};        /**< Rotation axis in local space of body B. */
        bool useLinearReferenceFrameA = true; /**< Whether to use the linear reference frame of body A. */
    };

    /**
     * @class PhysicsJoints
     * @brief Manages the creation and removal of physics constraints (joints).
     */
    class PhysicsJoints
    {
      public:
        /**
         * @brief Initializes the PhysicsJoints system.
         */
        static void Init();

        /**
         * @brief Creates a physics joint and stores it by ID.
         * @param id Identifier for the joint.
         * @param config Configuration parameters for the joint.
         * @return Pointer to the created joint constraint.
         */
        static btTypedConstraint* createJoint(const std::string& id, const JointConfig& config);

        /**
         * @brief Removes a physics joint by ID.
         * @param id Identifier of the joint to remove.
         */
        static void removeJoint(const std::string& id);

        /**
         * @brief Retrieves a joint by its ID.
         * @param id Identifier of the joint.
         * @return Pointer to the joint constraint, or nullptr if not found.
         */
        static btTypedConstraint* getJoint(const std::string& id);

        /**
         * @brief Adds all stored joints to the specified dynamics world.
         * @param world Pointer to the physics world.
         */
        static void addToWorld(btDynamicsWorld* world);

        /**
         * @brief Removes all stored joints from the specified dynamics world.
         * @param world Pointer to the physics world.
         */
        static void removeFromWorld(btDynamicsWorld* world);

        /**
         * @brief Destroys all joints and clears the stored data.
         */
        static void Destroy();

      private:
        static std::unordered_map<std::string, std::unique_ptr<btTypedConstraint>>
            joints_; /**< Storage for created joints. */

        /**
         * @brief Deletes all joints stored in the system.
         */
        static void deleteAllJoints();
    };

} // namespace Coffee
