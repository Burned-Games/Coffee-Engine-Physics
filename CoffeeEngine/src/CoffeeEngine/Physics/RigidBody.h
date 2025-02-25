#pragma once
#include "Collider.h"
#include "CollisionCallbacks.h"

namespace Coffee {

    enum class RigidBodyType {
        Static,     // Does not move, not affected by forces
        Dynamic,    // Moves and is affected by physics forces
        Kinematic   // Moves but is not affected by physics forces
    };

    struct RigidBodyConfig
    {
        CollisionShapeConfig shapeConfig;
        RigidBodyType type = RigidBodyType::Dynamic;
        bool UseGravity = true;                      ///< Whether the object is affected by gravity.
        glm::mat4 transform = glm::mat4(1.0f);
        glm::vec3 Velocity = {0.0f, 0.0f, 0.0f};     ///< The current velocity of the rigidbody.
        glm::vec3 Acceleration = {0.0f, 0.0f, 0.0f}; ///< The current acceleration of the rigidbody.

        // Physics Material
        float LinearDrag = 0.1f; ///< The linear drag of the rigidbody.
        float AngularDrag = 0.1f;

        // Constraints
        bool FreezeX = false;
        bool FreezeY = false;
        bool FreezeZ = false;
        bool FreezeRotX = false;
        bool FreezeRotY = false;
        bool FreezeRotZ = false;
    };
    
    class RigidBody {
    public:
        explicit RigidBody(RigidBodyConfig& config);
        ~RigidBody();

        void GetConfig(RigidBodyConfig& config);
        
        void ApplyForce(const glm::vec3& force, const glm::vec3& point = glm::vec3(0.0f));
        void ApplyImpulse(const glm::vec3& impulse, const glm::vec3& point = glm::vec3(0.0f));
        void UpdateGravity(const RigidBodyConfig& config);
        
    private:
        btRigidBody* m_RigidBody;
    
        CollisionCallbacks m_Callbacks;
        
        friend class PhysicsEngine;
    };

} // Coffee
