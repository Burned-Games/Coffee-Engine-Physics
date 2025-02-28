/**
 * @file PhysicsEngine.h
 * @brief Declares the PhysicsEngine class for managing physics simulation.
 */

#pragma once

#include "CoffeeEngine/Scene/Components.h"
#include "CoffeeEngine/Scene/Entity.h"
#include "Collider.h"
#include "CollisionCallbacks.h"
#include <bullet/btBulletDynamicsCommon.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <vector>

namespace Coffee
{

    class RigidbodyComponent;
    class TransformComponent;

    /**
     * @enum PhysicsType
     * @brief Defines different types of physics simulations.
     */
    enum class PhysicsType
    {
        BASIC,     ///< Basic physics simulation.
        DISCRETE,  ///< Discrete collision detection.
        PARALLEL,  ///< Parallel physics processing.
        CONTINUOUS ///< Continuous collision detection.
    };



    /**
     * @class PhysicsEngine
     * @brief Manages the physics world and updates simulation.
     */
    class PhysicsEngine
    {
      public:
        struct DebugDrawCommand
        {
            glm::vec3 position;      // position
            glm::quat rotation;      // rotation
            glm::vec3 size;          // Size
            glm::vec4 color;         // Color
            CollisionShapeType type; // Collider Type
        };


        /** @brief Initializes the physics engine. */
        static void Init();
        /** @brief Updates the physics simulation. */
        static void Update(float dt);
        /** @brief Destroys the physics engine and releases resources. */
        static void Destroy();
        /**
         * @brief Applies a rigid body to an entity's components.
         * @param rigidbodyComponent The rigid body component.
         * @param transformComponent The transform component.
         * @param dt Delta time.
         */
        static void ApplyRigidbody(RigidbodyComponent& rigidbodyComponent, TransformComponent& transformComponent,
                                   float dt);
        /** @brief Gets the physics world. */
        static btDynamicsWorld* GetWorld() { return m_world; }
        /** @brief Sets the gravity of the physics world. */
        static void SetGravity(const glm::vec3& gravity);
        /** @brief Gets the current gravity setting. */
        static glm::vec3 GetGravity();

        /** @brief Processes trigger events for collision detection. */
        static void ProcessTriggerEvents();
        /** @brief Sets the position of a collision object. */
        static void SetPosition(btCollisionObject* object, const glm::vec3& position);
        /** @brief Gets the position of a collision object. */
        glm::vec3 GetPosition(btCollisionObject* object);
        /** @brief Retrieves rigid body flags based on configuration. */
        static int GetRigidbodyFlags(const RigidBodyConfig& config);

        // Create + Destroy collision objects
        /* static btCollisionObject* CreateCollisionObject(const CollisionShapeConfig& config, const glm::vec3&
         * position);*/
        /** @brief Destroys a collision object. */
        static void DestroyCollisionObject(btCollisionObject* object);

        /** @brief Creates a collision shape based on configuration. */
        static btCollisionShape* CreateCollisionShape(const CollisionShapeConfig& config);

        

        static void AddDebugDrawCommand(CollisionShapeType type, const glm::vec3& position, const glm::quat& rotation,
                                        const glm::vec3& size,
                                        const glm::vec4& color = glm::vec4(0, 1, 0, 1));

        /**
         * @brief Creates a collision object.
         * @param config The collision shape configuration.
         * @param position The position of the object.
         * @param scale The scale of the object.
         * @param rotation The rotation of the object.
         * @return Pointer to the created collision object.
         */
        static btCollisionObject* CreateCollisionObject(const CollisionShapeConfig& config, const glm::vec3& position,
                                                        const glm::vec3& scale, const glm::quat& rotation);

        /**
         * @brief Creates a rigid body.
         * @param colCallbacks Pointer to collision callbacks.
         * @param config The rigid body configuration.
         * @return Pointer to the created rigid body.
         */
        static btRigidBody* CreateRigidBody(CollisionCallbacks* colCallbacks, const RigidBodyConfig& config);

        /** @brief Removes a rigid body from the physics world. */
        static void RemoveRigidBody(btRigidBody* rigidBody);


        static glm::vec3 GlobalGravity;
            


        static std::vector<DebugDrawCommand> debugDrawList;
      private:
        static btDynamicsWorld* m_world; ///< Pointer to the Bullet physics world.

        static btCollisionConfiguration* m_collision_conf; ///< Collision configuration.
        static btDispatcher* m_dispatcher;                 ///< Collision dispatcher.
        static btBroadphaseInterface* m_broad_phase;       ///< Broadphase collision detection.
        static btConstraintSolver* m_solver;               ///< Constraint solver.

        

        static std::vector<btCollisionObject*> m_CollisionObjects; ///< List of collision objects.
        static std::vector<btCollisionShape*> m_CollisionShapes;   ///< List of collision shapes.

        static std::vector<RigidBody*> m_Rigidbodies; ///< List of rigid bodies.

        friend class RigidBody; ///< Grant RigidBody access to private members.
    };
} // namespace Coffee
