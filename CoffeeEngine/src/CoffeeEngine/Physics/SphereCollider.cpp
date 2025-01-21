#include "SphereCollider.h"
#include "PhysUtils.h"
#include "PhysicsEngine.h"

namespace Coffee
{

    SphereCollider::SphereCollider(float radius, const glm::vec3& position, bool isStatic, bool isTrigger, float mass)
        : Collider(isStatic, isTrigger, mass), m_radius(radius)
    {
        UpdateCollisionShape();
        SetPosition(position);
    }

    SphereCollider::~SphereCollider()
    {
        // La lógica de limpieza está en la clase base
    }

    void SphereCollider::SetRadius(float radius)
    {
        if (m_radius != radius)
        {
            m_radius = radius;
            UpdateCollisionShape();
        }
    }

    float SphereCollider::GetRadius() const
    {
        return m_radius;
    }

    void SphereCollider::UpdateCollisionShape()
    {
        btCollisionShape* shape = new btSphereShape(m_radius);
        m_collisionObject->setCollisionShape(shape);
    }

} // namespace Coffee
