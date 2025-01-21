#pragma once

#include "Collider.h"
#include "PhysicsEngine.h"

namespace Coffee
{

    class BoxCollider : public Collider
    {
      public:
        BoxCollider(const glm::vec3& size, const glm::vec3& position, bool isStatic = false, bool isTrigger = false,
                    float mass = 1.0f);
        ~BoxCollider();

        // Setters and getters for position
        void SetPosition(const glm::vec3& position);
        glm::vec3 GetPosition() const;

        // Enable or disable the collider
        void SetEnabled(bool enabled);
        bool IsEnabled() const;

        // Access underlying Bullet collision object
        btCollisionObject* GetCollisionObject() const { return m_collisionObject; }

      private:
        btCollisionObject* m_collisionObject;
        glm::vec3 m_size;
        bool m_isStatic;
        bool m_isTrigger;
        float m_mass;

        void UpdateCollisionObject();
    };

} // namespace Coffee
