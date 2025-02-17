#include "Collider.h"
#include "PhysUtils.h"
#include "PhysicsEngine.h"

namespace Coffee
{

    Collider::Collider(bool isStatic, bool isTrigger, float mass)
        : m_isStatic(isStatic), m_isTrigger(isTrigger), m_mass(mass), m_position(0.0f)
    {
        m_collisionObject = new btCollisionObject();
        if (m_isTrigger)
        {
            m_collisionObject->setCollisionFlags(m_collisionObject->getCollisionFlags() |
                                                 btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }
        PhysicsEngine::GetWorld()->addCollisionObject(m_collisionObject);
    }

    Collider::~Collider()
    {
        PhysicsEngine::GetWorld()->removeCollisionObject(m_collisionObject);
        delete m_collisionObject;
    }

    void Collider::SetPosition(const glm::vec3& position)
    {
        m_position = position;
        btTransform transform = m_collisionObject->getWorldTransform();
        transform.setOrigin(PhysUtils::GlmToBullet(m_position));
        m_collisionObject->setWorldTransform(transform);
    }

    glm::vec3 Collider::GetPosition() const
    {
        return m_position;
    }

    void Collider::SetEnabled(bool enabled)
    {
        if (enabled)
        {
            m_collisionObject->setCollisionFlags(m_collisionObject->getCollisionFlags() &
                                                 ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }
        else
        {
            m_collisionObject->setCollisionFlags(m_collisionObject->getCollisionFlags() |
                                                 btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }
    }

    bool Collider::IsEnabled() const
    {
        return !(m_collisionObject->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE);
    }

    void Collider::AddCollisionListener(const CollisionCallback& callback)
    {
        m_collisionListeners.push_back(callback);
    }

    void Collider::OnCollision(Collider* other)
    {
        for (auto& listener : m_collisionListeners)
        {
            listener(other);
        }
    }

} // namespace Coffee
