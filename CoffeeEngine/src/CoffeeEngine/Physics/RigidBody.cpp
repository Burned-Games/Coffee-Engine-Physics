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

    void RigidBody::ApplyShape(btCollisionShape* shape, glm::vec3 position,
                            glm::quat rotation,
                            glm::vec3 size)
    {
        if (!m_RigidBody || !shape)
            return;

        // Remover el rigidbody del mundo antes de modificarlo
        PhysicsEngine::GetWorld()->removeRigidBody(m_RigidBody);

        // Calcular la nueva inercia basada en la nueva forma
        btVector3 localInertia(0, 0, 0);
        shape->calculateLocalInertia(1.0f, localInertia);

        // Crear la nueva transformación con los valores pasados
        btTransform newTransform;
        newTransform.setIdentity();
        newTransform.setOrigin(PhysUtils::GlmToBullet(position));
        newTransform.setRotation(PhysUtils::GlmToBullet(rotation));

        // Si el shape es un btBoxShape, ajustar la escala manualmente
        /*btBoxShape* boxShape = dynamic_cast<btBoxShape*>(shape);
        if (boxShape)
        {
            boxShape->setImplicitShapeDimensions(PhysUtils::GlmToBullet(size * 0.5f));
        }*/

        // Crear el nuevo motion state con la transformación proporcionada
        btDefaultMotionState* motionState = new btDefaultMotionState(newTransform);

        // Crear la nueva configuración del rigidbody con la forma y transformación correctas
        btRigidBody::btRigidBodyConstructionInfo rbInfo(1.0f, motionState, shape, localInertia);
        btRigidBody* newBody = new btRigidBody(rbInfo);

        // Mantener las propiedades del objeto
        newBody->setFlags(newBody->getFlags() | btCollisionObject::CF_DYNAMIC_OBJECT);
        newBody->setUserPointer(&m_Callbacks);

        // Agregarlo de nuevo al mundo físico
        PhysicsEngine::GetWorld()->addRigidBody(newBody);

        // Reemplazar el puntero del rigidbody anterior con el nuevo
        m_RigidBody = newBody;
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