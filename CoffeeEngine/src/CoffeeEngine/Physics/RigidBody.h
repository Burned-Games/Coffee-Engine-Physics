#pragma once

#include "CollisionCallbacks.h"
#include "PhysicsEngine.h"

namespace Coffee {

    class RigidBody {
    public:
        explicit RigidBody(const RigidBodyConfig& config);
        ~RigidBody();
        
    private:
        btRigidBody* m_RigidBody;
    
        CollisionCallbacks m_Callbacks;
        
        friend class PhysicsEngine;
    };

} // Coffee
