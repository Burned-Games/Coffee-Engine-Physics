#pragma once

#include "Collider.h"

namespace Coffee
{

    class BoxCollider : public Collider
    {
      public:
        BoxCollider(const glm::vec3& size, const glm::vec3& position, bool isStatic = false, bool isTrigger = false,
                    float mass = 1.0f);
        ~BoxCollider();

      protected:
        void UpdateCollisionShape() override;

      private:
        glm::vec3 m_size;
    };

} // namespace Coffee
