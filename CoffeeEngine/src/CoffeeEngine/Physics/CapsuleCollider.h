#pragma once

#include "Collider.h"

namespace Coffee
{

    class CapsuleCollider : public Collider
    {
      public:
        CapsuleCollider(float radius, float height, const glm::vec3& position, bool isStatic = false,
                        bool isTrigger = false, float mass = 1.0f);
        ~CapsuleCollider();

        // Setters and getters for radius and height
        void SetRadius(float radius);
        float GetRadius() const;

        void SetHeight(float height);
        float GetHeight() const;

      protected:
        void UpdateCollisionShape() override;

      private:
        float m_radius;
        float m_height;
    };

} // namespace Coffee
