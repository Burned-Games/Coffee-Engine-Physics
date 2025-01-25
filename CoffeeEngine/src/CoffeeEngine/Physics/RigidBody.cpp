#include "RigidBody.h"

namespace Coffee {
    RigidBody::RigidBody(const RigidBodyConfig& config)
    {
        m_Callbacks.rigidBody = this;
        this->m_RigidBody = PhysicsEngine::CreateRigidBody(&m_Callbacks, config);
    }
    
    RigidBody::~RigidBody()
    {
        PhysicsEngine::RemoveRigidBody(m_RigidBody);
        delete m_RigidBody->getCollisionShape();
    }
} // Coffee