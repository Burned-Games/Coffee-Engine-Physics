#pragma once

#include "Collider.h"

namespace Coffee
{

    class PlaneCollider : public Collider
    {
      public:
        PlaneCollider(const glm::vec3& normal, float constant, const glm::vec3& position = glm::vec3(0.0f));
        ~PlaneCollider();

        // Setters and getters for plane properties
        void SetNormal(const glm::vec3& normal);
        glm::vec3 GetNormal() const;

        void SetConstant(float constant);
        float GetConstant() const;

      protected:
        void UpdateCollisionShape() override;

      private:
        glm::vec3 m_normal; // Normal vector defining the plane
        float m_constant;   // Distance of the plane from the origin along its normal
    };

} // namespace Coffee
