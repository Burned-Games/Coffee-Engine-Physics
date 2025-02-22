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
        

    };

    class Collider
    {
      public:
        using CollisionCallback = std::function<void(Collider* other)>;

        Collider(bool isTrigger, float mass);
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
        bool m_isTrigger;
        float m_mass;

      private:
        std::vector<CollisionCallback> m_collisionListeners;
    };

    class BoxCollider : public Collider
    {
      public:
        BoxCollider(const glm::vec3& size, const glm::vec3& position, bool isTrigger = false,
                    float mass = 1.0f);
        ~BoxCollider();

      protected:
        void UpdateCollisionShape() override;

      private:
        glm::vec3 m_size;
    };

    class CapsuleCollider : public Collider
    {
      public:
        CapsuleCollider(float radius, float height, const glm::vec3& position,
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

    class CylinderCollider : public Collider
    {
      public:
        CylinderCollider(const glm::vec3& dimensions, const glm::vec3& position,
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

    class SphereCollider : public Collider
    {
      public:
        SphereCollider(float radius, const glm::vec3& position, bool isTrigger = false,
                       float mass = 1.0f);
        ~SphereCollider();

        // Setter and getter for radius
        void SetRadius(float radius);
        float GetRadius() const;

      protected:
        void UpdateCollisionShape() override;

      private:
        float m_radius;
    };

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
