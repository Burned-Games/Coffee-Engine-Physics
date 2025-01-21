#include "BoxCollider.h"
#include "PhysUtils.h"

namespace Coffee
{

    BoxCollider::BoxCollider(const glm::vec3& size, const glm::vec3& position, bool isStatic, bool isTrigger,
                             float mass)
        : m_size(size), m_isStatic(isStatic), m_isTrigger(isTrigger), m_mass(mass), m_collisionObject(nullptr)
    {
        CollisionShapeConfig config;
        config.type = CollisionShapeType::BOX;
        config.size = size;
        config.isStatic = isStatic;
        config.isTrigger = isTrigger;
        config.mass = mass;

        m_collisionObject = PhysicsEngine::CreateCollisionObject(config, position);
        m_collisionObject->setUserPointer(this); // Asocia este BoxCollider al objeto de Bullet
    }

    BoxCollider::~BoxCollider()
    {
        if (m_collisionObject)
        {
            PhysicsEngine::DestroyCollisionObject(m_collisionObject);
            m_collisionObject = nullptr;
        }
    }

    void BoxCollider::SetPosition(const glm::vec3& position)
    {
        if (m_collisionObject)
        {
            btTransform transform = m_collisionObject->getWorldTransform();
            transform.setOrigin(PhysUtils::GlmToBullet(position));
            m_collisionObject->setWorldTransform(transform);

            // Si es un rigid body, actualiza el estado de movimiento
            btRigidBody* body = btRigidBody::upcast(m_collisionObject);
            if (body && body->getMotionState())
            {
                body->getMotionState()->setWorldTransform(transform);
            }
        }
    }

    glm::vec3 BoxCollider::GetPosition() const
    {
        if (m_collisionObject)
        {
            btTransform transform = m_collisionObject->getWorldTransform();
            return PhysUtils::BulletToGlm(transform.getOrigin());
        }
        return glm::vec3(0.0f);
    }

    void BoxCollider::SetEnabled(bool enabled)
    {
        if (m_collisionObject)
        {
            if (enabled)
            {
                PhysicsEngine::GetWorld()->addCollisionObject(m_collisionObject);
            }
            else
            {
                PhysicsEngine::GetWorld()->removeCollisionObject(m_collisionObject);
            }
        }
    }

    bool BoxCollider::IsEnabled() const
    {
        if (m_collisionObject)
        {
            return PhysicsEngine::GetWorld()->getCollisionObjectArray().findLinearSearch(m_collisionObject) != -1;
        }
        return false;
    }

    void BoxCollider::UpdateCollisionObject()
    {
        if (m_collisionObject)
        {
            // Actualiza las propiedades del colisionador si es necesario
            // Por ejemplo, si cambian las dimensiones o masa
            m_collisionObject->setCollisionShape(PhysicsEngine::CreateCollisionShape(
                {CollisionShapeType::BOX, m_size, m_isTrigger, m_mass, m_isStatic}));
        }
    }

} // namespace Coffee
