#include "CylinderCollider.h"
#include "PhysUtils.h"
#include "PhysicsEngine.h"

namespace Coffee
{

    CylinderCollider::CylinderCollider(const glm::vec3& dimensions, const glm::vec3& position, bool isStatic,
                                       bool isTrigger, float mass)
        : Collider(isStatic, isTrigger, mass), m_dimensions(dimensions)
    {
        UpdateCollisionShape();
        SetPosition(position);
    }

    CylinderCollider::~CylinderCollider()
    {
        // La lógica de limpieza está en la clase base
    }

    void CylinderCollider::SetDimensions(const glm::vec3& dimensions)
    {
        if (m_dimensions != dimensions)
        {
            m_dimensions = dimensions;
            UpdateCollisionShape();
        }
    }

    glm::vec3 CylinderCollider::GetDimensions() const
    {
        return m_dimensions;
    }

    void CylinderCollider::UpdateCollisionShape()
    {
        // Crear una nueva forma de colisión para el cilindro
        btVector3 halfExtents = PhysUtils::GlmToBullet(m_dimensions * 0.5f); // Convertir dimensiones a half extents
        btCollisionShape* shape = new btCylinderShape(halfExtents);
        m_collisionObject->setCollisionShape(shape);
    }

} // namespace Coffee
