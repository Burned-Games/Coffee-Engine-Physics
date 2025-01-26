#pragma once

#include "Collider.h"

namespace Coffee
{

    class CylinderCollider : public Collider
    {
      public:
        CylinderCollider(const glm::vec3& dimensions, const glm::vec3& position, bool isStatic = false,
                         bool isTrigger = false, float mass = 1.0f);
        ~CylinderCollider();

        // Setters and getters for dimensions
        void SetDimensions(const glm::vec3& dimensions);
        glm::vec3 GetDimensions() const;

      protected:
        void UpdateCollisionShape() override;

      private:
        glm::vec3 m_dimensions; // Half extents: (width/2, height/2, depth/2)
    };

} // namespace Coffee
