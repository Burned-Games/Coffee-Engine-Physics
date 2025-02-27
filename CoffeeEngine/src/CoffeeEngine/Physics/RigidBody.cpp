#include "RigidBody.h"

#include "PhysUtils.h"
#include "PhysicsEngine.h"

#include <glm/gtc/type_ptr.hpp>

namespace Coffee {
    RigidBody::RigidBody(RigidBodyConfig& config)
    {
        m_Callbacks.rigidBody = this;
        this->m_RigidBody = PhysicsEngine::CreateRigidBody(&m_Callbacks, config);
        
        UpdateGravity(config);
    }
    
    RigidBody::~RigidBody()
    {
        PhysicsEngine::RemoveRigidBody(m_RigidBody);
        delete m_RigidBody->getMotionState();
        delete m_RigidBody;
    }
    void RigidBody::GetConfig(RigidBodyConfig& config)
    {
        int flags = m_RigidBody->getFlags();
        if (flags & btCollisionObject::CF_STATIC_OBJECT)
            config.type = RigidBodyType::Static;
        else if (flags & btCollisionObject::CF_KINEMATIC_OBJECT)
            config.type = RigidBodyType::Kinematic;
        else
            config.type = RigidBodyType::Dynamic;

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

    void RigidBody::ApplyForce(const glm::vec3& force, const glm::vec3& point)
    {
        if (!m_RigidBody) return;
        
        btVector3 btForce = PhysUtils::GlmToBullet(force);
        btVector3 btPoint = PhysUtils::GlmToBullet(point);
        
        m_RigidBody->applyForce(btForce, btPoint);
        m_RigidBody->activate(true);
    }

    void RigidBody::ApplyImpulse(const glm::vec3& impulse, const glm::vec3& point)
    {
        if (!m_RigidBody) return;
        
        btVector3 btImpulse = PhysUtils::GlmToBullet(impulse);
        btVector3 btPoint = PhysUtils::GlmToBullet(point);
        
        m_RigidBody->applyImpulse(btImpulse, btPoint);
        m_RigidBody->activate(true);
    }

    void RigidBody::ApplyShape(btCollisionShape* shape)
    {
        if (!m_RigidBody)
            return;

        m_RigidBody->setCollisionShape(shape);

        
    }

    void RigidBody::UpdateGravity(const RigidBodyConfig& config)
    {
        if (m_RigidBody)
        {
            if (!config.UseGravity || config.type != RigidBodyType::Dynamic) {
                m_RigidBody->setGravity(btVector3(0, 0, 0));
            }
            else {
                m_RigidBody->setGravity(PhysUtils::GlmToBullet(PhysicsEngine::GetGravity()));
            }
        }
    }

    void RigidBody::SetVelocity(const glm::vec3& velocity)
    {
        if (m_RigidBody)
        {
            m_RigidBody->setLinearVelocity(PhysUtils::GlmToBullet(velocity));
            m_RigidBody->activate(true);
        }
    }

    void RigidBody::AddVelocity(const glm::vec3& velocity)
    {
        if (m_RigidBody)
        {
            btVector3 currentVel = m_RigidBody->getLinearVelocity();
            btVector3 additionalVel = PhysUtils::GlmToBullet(velocity);
            m_RigidBody->setLinearVelocity(currentVel + additionalVel);
            m_RigidBody->activate(true);
        }
    }

    void RigidBody::SetAngularVelocity(const glm::vec3& angularVelocity)
    {
        if (m_RigidBody)
        {
            m_RigidBody->setAngularVelocity(PhysUtils::GlmToBullet(angularVelocity));
            m_RigidBody->activate(true);
        }
    }

    glm::vec3 RigidBody::GetVelocity() const 
    {
        if (m_RigidBody)
        {
            return PhysUtils::BulletToGlm(m_RigidBody->getLinearVelocity());
        }
        return glm::vec3(0.0f);
    }

    glm::vec3 RigidBody::GetAngularVelocity() const
    {
        if (m_RigidBody)
        {
            return PhysUtils::BulletToGlm(m_RigidBody->getAngularVelocity());
        }
        return glm::vec3(0.0f);
    }
} // Coffee