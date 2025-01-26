#pragma once

#include <bullet/btBulletDynamicsCommon.h>
#include <functional>
#include <glm/glm.hpp>
#include <vector>

namespace Coffee
{

    // collision shape types
    enum class CollisionShapeType {
        BOX,
        SPHERE,
        CAPSULE,
        CYLINDER,
        MESH
    };

    struct CollisionShapeConfig {
        CollisionShapeType type = CollisionShapeType::BOX;
        glm::vec3 size = glm::vec3(1.0f);  // size of the shape
        bool isTrigger = false;
        float mass = 1.0f;
        bool isStatic = false; // mass == 0 means static object
    };

    class Collider
    {
      public:
        using CollisionCallback = std::function<void(Collider* other)>;

        Collider(bool isStatic, bool isTrigger, float mass);
        virtual ~Collider();

        // Position management
        void SetPosition(const glm::vec3& position);
        glm::vec3 GetPosition() const;

        // Enable/disable collider
        void SetEnabled(bool enabled);
        bool IsEnabled() const;

        // Add collision listeners
        void AddCollisionListener(const CollisionCallback& callback);

        // Trigger collision callbacks
        void OnCollision(Collider* other);

        // Access Bullet collision object
        btCollisionObject* GetCollisionObject() const { return m_collisionObject; }

      protected:
        virtual void UpdateCollisionShape() = 0; // Implementado por clases derivadas

        btCollisionObject* m_collisionObject;
        glm::vec3 m_position;
        bool m_isStatic;
        bool m_isTrigger;
        float m_mass;

      private:
        std::vector<CollisionCallback> m_collisionListeners;
    };

} // namespace Coffee
