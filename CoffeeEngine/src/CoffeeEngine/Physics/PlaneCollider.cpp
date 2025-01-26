#include "PlaneCollider.h"
#include "PhysUtils.h"
#include "PhysicsEngine.h"

namespace Coffee
{

    PlaneCollider::PlaneCollider(const glm::vec3& normal, float constant, const glm::vec3& position)
        : Collider(true, false, 0.0f), m_normal(normal), m_constant(constant)
    {
        UpdateCollisionShape();
        SetPosition(position);
    }

    PlaneCollider::~PlaneCollider()
    {
        // La lógica de limpieza está en la clase base
    }

    void PlaneCollider::SetNormal(const glm::vec3& normal)
    {
        if (m_normal != normal)
        {
            m_normal = normal;
            UpdateCollisionShape();
        }
    }

    glm::vec3 PlaneCollider::GetNormal() const
    {
        return m_normal;
    }

    void PlaneCollider::SetConstant(float constant)
    {
        if (m_constant != constant)
        {
            m_constant = constant;
            UpdateCollisionShape();
        }
    }

    float PlaneCollider::GetConstant() const
    {
        return m_constant;
    }

    void PlaneCollider::UpdateCollisionShape()
    {
        // Crear una nueva forma de colisión para el plano
        btCollisionShape* shape = new btStaticPlaneShape(PhysUtils::GlmToBullet(m_normal), m_constant);
        m_collisionObject->setCollisionShape(shape);
    }

} // namespace Coffee
