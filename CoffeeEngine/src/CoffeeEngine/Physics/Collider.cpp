#include "Collider.h"

namespace Coffee
{

    Collider::Collider() {}

    Collider::~Collider() {}

    void Collider::AddCollisionListener(const CollisionCallback& callback)
    {
        m_collisionListeners.push_back(callback);
    }

    void Collider::OnCollision(Collider* other)
    {
        for (const auto& callback : m_collisionListeners)
        {
            callback(other);
        }
    }

} // namespace Coffee
