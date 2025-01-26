#include "CapsuleCollider.h"
#include "PhysUtils.h"
#include "PhysicsEngine.h"

namespace Coffee
{

    CapsuleCollider::CapsuleCollider(float radius, float height, const glm::vec3& position, bool isStatic,
                                     bool isTrigger, float mass)
        : Collider(isStatic, isTrigger, mass), m_radius(radius), m_height(height)
    {
        UpdateCollisionShape();
        SetPosition(position);
    }

    CapsuleCollider::~CapsuleCollider()
    {
        // La lógica de limpieza está en la clase base
    }

    void CapsuleCollider::SetRadius(float radius)
    {
        if (m_radius != radius)
        {
            m_radius = radius;
            UpdateCollisionShape();
        }
    }

    float CapsuleCollider::GetRadius() const
    {
        return m_radius;
    }

    void CapsuleCollider::SetHeight(float height)
    {
        if (m_height != height)
        {
            m_height = height;
            UpdateCollisionShape();
        }
    }

    float CapsuleCollider::GetHeight() const
    {
        return m_height;
    }

    void CapsuleCollider::UpdateCollisionShape()
    {
        // Crear una nueva forma de colisión para la cápsula
        btCollisionShape* shape = new btCapsuleShape(m_radius, m_height);
        m_collisionObject->setCollisionShape(shape);
    }

} // namespace Coffee
