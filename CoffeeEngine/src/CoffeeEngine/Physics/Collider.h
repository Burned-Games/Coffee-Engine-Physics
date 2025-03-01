#pragma once

#include <bullet/btBulletDynamicsCommon.h>
#include <functional>
#include <glm/glm.hpp>
#include <vector>
#include <glm/gtc/quaternion.hpp>

namespace Coffee
{

    /**
     * @enum CollisionShapeType
     * @brief Defines the types of collision shapes.
     */
    enum class CollisionShapeType
    {
        BOX,      /**< Box shape */
        SPHERE,   /**< Sphere shape */
        CAPSULE,  /**< Capsule shape */
        CYLINDER, /**< Cylinder shape */
        MESH      /**< Mesh shape */
    };

    /**
     * @struct CollisionShapeConfig
     * @brief Configuration for a collision shape.
     */
    struct CollisionShapeConfig
    {
        CollisionShapeType type = CollisionShapeType::BOX; /**< Type of the collision shape */
        glm::vec3 size = glm::vec3(1.0f);                  /**< Size of the shape */
        bool isTrigger = false;                            /**< Whether the shape is a trigger */
        float mass = 1.0f;                                 /**< Mass of the object */
    };

    /**
     * @class Collider
     * @brief Base class for all colliders.
     */
    class Collider
    {

      public:
        using CollisionCallback = std::function<void(Collider* other)>;

        /**
         * @brief Constructor for Collider.
         * @param isTrigger Whether the collider is a trigger.
         * @param mass The mass of the collider.
         */
        Collider(const CollisionShapeConfig& config, const glm::vec3& position, const glm::quat& rotation,
                 const glm::vec3& scale);

        /**
         * @brief Destructor for Collider.
         */
        ~Collider();

        /**
         * @brief Sets the position of the collider.
         * @param position The new position.
         */
        // void SetPosition(const glm::vec3& position, const glm::vec3& offset = glm::vec3(0,0,0));

        void ColliderUpdate(const glm::vec3 position = glm::vec3(0, 0, 0),
                            const glm::quat rotation = glm::quat(1, 0, 0, 0), 
                            const glm::vec3 size = glm::vec3(0, 0, 0),
                            const glm::vec3 offset = glm::vec3(1, 0, 0));

        /**
         * @brief Gets the current position of the collider.
         * @return The collider's position.
         */
        void SetSize(const glm::vec3& size, const glm::vec3& sizeOffset);
        glm::vec3 GetPosition() const;
        glm::quat GetRotation() const;
        glm::vec3 GetSize() const;

        btCollisionShape* GetShape();
        /**
         * @brief Enables or disables the collider.
         * @param enabled True to enable, false to disable.
         */
        void SetEnabled(bool enabled);

        /**
         * @brief Checks if the collider is enabled.
         * @return True if enabled, false otherwise.
         */
        bool IsEnabled() const;

        /**
         * @brief Adds a collision listener.
         * @param callback Function to execute upon collision.
         */
        void AddCollisionListener(const CollisionCallback& callback);

        /**
         * @brief Handles collision events.
         * @param other The other collider involved.
         */
        void OnCollision(Collider* other);

        /**
         * @brief Gets the Bullet collision object.
         * @return Pointer to the Bullet collision object.
         */
        btCollisionObject* GetCollisionObject() const { return m_collisionObject; }

      protected:
        void UpdateCollisionShape(); /**< Implemented by derived classes */

        btCollisionObject* m_collisionObject; /**< Bullet collision object */
        glm::vec3 m_position;                 /**< Collider position */
        glm::quat m_rotation;                 /**< Collider rotation */
        glm::vec3 m_scale;                    /**< Collider scale */
        glm::vec3 m_offset;                   /**< Collider offset  */

        bool m_isTrigger; /**< Whether the collider is a trigger */
        float m_mass;     /**< Mass of the collider */

      private:
        std::vector<CollisionCallback> m_collisionListeners; /**< List of collision listeners */
    };

    /**
     * @class BoxCollider
     * @brief Represents a box-shaped collider.
     */
    // class BoxCollider : public Collider
    //{
    //   public:
    //     BoxCollider(const glm::vec3& size, const glm::vec3& position, bool isTrigger = false,
    //                 float mass = 1.0f);
    //     ~BoxCollider();

    //  protected:
    //    void UpdateCollisionShape() override;

    //  private:
    //    glm::vec3 m_size; /**< Size of the box */
    //};

    ///**
    // * @class CapsuleCollider
    // * @brief Represents a capsule-shaped collider.
    // */
    // class CapsuleCollider : public Collider
    //{
    //  public:
    //    CapsuleCollider(float radius, float height, const glm::vec3& position,
    //                    bool isTrigger = false, float mass = 1.0f);
    //    ~CapsuleCollider();

    //    // Setters and getters for radius and height
    //    void SetRadius(float radius);
    //    float GetRadius() const;

    //    void SetHeight(float height);
    //    float GetHeight() const;

    //  protected:
    //    void UpdateCollisionShape() override;

    //  private:
    //    float m_radius; /**< Radius of the capsule */
    //    float m_height; /**< Height of the capsule */
    //};

    ///**
    // * @class CylinderCollider
    // * @brief Represents a cylinder-shaped collider.
    // */
    // class CylinderCollider : public Collider
    //{
    //  public:
    //    CylinderCollider(const glm::vec3& dimensions, const glm::vec3& position,
    //                     bool isTrigger = false, float mass = 1.0f);
    //    ~CylinderCollider();

    //    // Setters and getters for dimensions
    //    void SetDimensions(const glm::vec3& dimensions);
    //    glm::vec3 GetDimensions() const;

    //  protected:
    //    void UpdateCollisionShape() override;

    //  private:
    //    glm::vec3 m_dimensions; /**< Half extents of the cylinder */
    //};

    ///**
    // * @class SphereCollider
    // * @brief Represents a sphere-shaped collider.
    // */
    // class SphereCollider : public Collider
    //{
    //  public:
    //    SphereCollider(float radius, const glm::vec3& position, bool isTrigger = false,
    //                   float mass = 1.0f);
    //    ~SphereCollider();

    //    // Setter and getter for radius
    //    void SetRadius(float radius);
    //    float GetRadius() const;

    //  protected:
    //    void UpdateCollisionShape() override;

    //  private:
    //    float m_radius;   /**< Radius of the sphere */
    //};

    ///**
    // * @class PlaneCollider
    // * @brief Represents a plane collider.
    // */
    // class PlaneCollider : public Collider
    //{
    //  public:
    //    PlaneCollider(const glm::vec3& normal, float constant, const glm::vec3& position = glm::vec3(0.0f));
    //    ~PlaneCollider();

    //    // Setters and getters for plane properties
    //    void SetNormal(const glm::vec3& normal);
    //    glm::vec3 GetNormal() const;

    //    void SetConstant(float constant);
    //    float GetConstant() const;

    //  protected:
    //    void UpdateCollisionShape() override;

    //  private:
    //    glm::vec3 m_normal; /**< Normal vector defining the plane */
    //    float m_constant;   /**< Distance of the plane from the origin */
    //};

} // namespace Coffee
