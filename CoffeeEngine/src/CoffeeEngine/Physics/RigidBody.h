#pragma once
#include "Collider.h"
#include "CollisionCallbacks.h"

namespace Coffee {

    struct RigidBodyConfig
    {
        CollisionShapeConfig shapeConfig;
        bool IsStatic = false;
        bool IsKinematic = false;                    ///< Whether the object is static (non-moving) or dynamic (moving).
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
        
    private:
        btRigidBody* m_RigidBody;
    
        CollisionCallbacks m_Callbacks;
        
        friend class PhysicsEngine;
    };

} // Coffee
