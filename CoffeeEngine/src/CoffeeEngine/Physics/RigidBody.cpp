#include "RigidBody.h"

#include "PhysUtils.h"
#include "PhysicsEngine.h"

#include <glm/gtc/type_ptr.hpp>

namespace Coffee {
    RigidBody::RigidBody(const RigidBodyConfig& config)
    {
        m_Callbacks.rigidBody = this;
        this->m_RigidBody = PhysicsEngine::CreateRigidBody(&m_Callbacks, config);
    }
    
    RigidBody::~RigidBody()
    {
        PhysicsEngine::RemoveRigidBody(m_RigidBody);
        delete m_RigidBody->getMotionState();
        delete m_RigidBody->getCollisionShape();
        delete m_RigidBody;
    }
    void RigidBody::GetConfig(RigidBodyConfig& config)
    {
        // TODO leer tambien la shape
        config.IsKinematic = m_RigidBody->getFlags() & btRigidBody::CF_KINEMATIC_OBJECT;
        config.UseGravity = m_RigidBody->getGravity() != btVector3(0, 0, 0);
        // Read transform
        config.transform = PhysUtils::Mat4BulletToGlm(m_RigidBody->getWorldTransform());
        config.Velocity = PhysUtils::BulletToGlm(m_RigidBody->getLinearVelocity());
        config.Acceleration = PhysUtils::BulletToGlm(m_RigidBody->getTotalForce());
        config.LinearDrag = m_RigidBody->getLinearDamping();
        config.AngularDrag = m_RigidBody->getAngularDamping();
        // Constraints
        // TODO usar joints para esto? No hay opcion para ello en el rigidbody
        
    }
} // Coffee