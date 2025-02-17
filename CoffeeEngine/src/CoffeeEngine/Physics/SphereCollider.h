#pragma once

#include "Collider.h"

namespace Coffee {

    class SphereCollider : public Collider {
    public:
        SphereCollider(float radius, const glm::vec3& position, bool isStatic = false, bool isTrigger = false, float mass = 1.0f);
        ~SphereCollider();

        // Setter and getter for radius
        void SetRadius(float radius);
        float GetRadius() const;

    protected:
        void UpdateCollisionShape() override;

    private:
        float m_radius;
    };

} // namespace Coffee
