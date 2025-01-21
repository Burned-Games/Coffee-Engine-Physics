#include "BoxCollider.h"
#include "PhysUtils.h"
#include "PhysicsEngine.h"

namespace Coffee
{

    BoxCollider::BoxCollider(const glm::vec3& size, const glm::vec3& position, bool isStatic, bool isTrigger,
                             float mass)
        : Collider(isStatic, isTrigger, mass), m_size(size)
    {
        UpdateCollisionShape();
        SetPosition(position);
    }

    BoxCollider::~BoxCollider()
    {
        // La lógica de limpieza está en la clase base
    }

    void BoxCollider::UpdateCollisionShape()
    {
        btCollisionShape* shape = new btBoxShape(PhysUtils::GlmToBullet(m_size * 0.5f));
        m_collisionObject->setCollisionShape(shape);
    }

} // namespace Coffee
