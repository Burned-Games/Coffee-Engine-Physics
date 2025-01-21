#pragma once

#include <functional>
#include <vector>

namespace Coffee
{

    class Collider
    {
      public:
        Collider();
        virtual ~Collider();

        // Callback de colisión: se llama cuando este colisionador interactúa con otro
        using CollisionCallback = std::function<void(Collider* other)>;

        void AddCollisionListener(const CollisionCallback& callback);
        void OnCollision(Collider* other);

      private:
        std::vector<CollisionCallback> m_collisionListeners;
    };

} // namespace Coffee
