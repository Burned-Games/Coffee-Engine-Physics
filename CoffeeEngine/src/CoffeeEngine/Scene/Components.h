/**
 * @defgroup scene Scene
 * @{
 */

#pragma once

#include "CoffeeEngine/Core/Base.h"
#include "CoffeeEngine/IO/ResourceLoader.h"
#include "CoffeeEngine/IO/ResourceRegistry.h"
#include "CoffeeEngine/Renderer/Material.h"
#include "CoffeeEngine/Renderer/Mesh.h"
#include "CoffeeEngine/Renderer/Model.h"
#include "CoffeeEngine/Scene/SceneCamera.h"
#include "src/CoffeeEngine/IO/Serialization/GLMSerialization.h"
#include "src/CoffeeEngine/IO/Serialization/BulletSerialization.h"
#include "CoffeeEngine/Physics/PhysicsEngine.h"
#include "src/CoffeeEngine/Physics/Collider.h"
#include <cereal/access.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include "CoffeeEngine/Physics/RigidBody.h"

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Coffee
{
    
    /**
     * @brief Component representing a tag.
     * @ingroup scene
     */
    struct TagComponent
    {
        std::string Tag; ///< The tag string.

        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& tag) : Tag(tag) {}

        /**
         * @brief Serializes the TagComponent.
         * @tparam Archive The type of the archive.
         * @param archive The archive to serialize to.
         */
        template <class Archive> void serialize(Archive& archive) { archive(cereal::make_nvp("Tag", Tag)); }
    };

    /**
     * @brief Component representing a transform.
     * @ingroup scene
     */
    struct TransformComponent
    {
      private:
        glm::mat4 worldMatrix = glm::mat4(1.0f); ///< The world transformation matrix.
      public:
        glm::vec3 Position = {0.0f, 0.0f, 0.0f}; ///< The position vector.
        glm::vec3 Rotation = {0.0f, 0.0f, 0.0f}; ///< The rotation vector.
        glm::vec3 Scale = {1.0f, 1.0f, 1.0f};    ///< The scale vector.

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::vec3& position) : Position(position) {}

        /**
         * @brief Gets the local transformation matrix.
         * @return The local transformation matrix.
         */
        glm::mat4 GetLocalTransform() const
        {
            glm::mat4 rotation = glm::toMat4(glm::quat(glm::radians(Rotation)));

            return glm::translate(glm::mat4(1.0f), Position) * rotation * glm::scale(glm::mat4(1.0f), Scale);
        }

        /**
         * @brief Sets the local transformation matrix.
         * @param transform The transformation matrix to set.
         */
        void SetLocalTransform(
            const glm::mat4& transform) // TODO: Improve this function, this way is ugly and glm::decompose is from gtx
                                        // (is supposed to not be very stable)
        {
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::quat orientation;

            glm::decompose(transform, Scale, orientation, Position, skew, perspective);
            Rotation = glm::degrees(glm::eulerAngles(orientation));
        }

        /**
         * @brief Gets the world transformation matrix.
         * @return The world transformation matrix.
         */
        const glm::mat4& GetWorldTransform() const { return worldMatrix; }

        /**
         * @brief Sets the world transformation matrix.
         * @param transform The transformation matrix to set.
         */
        void SetWorldTransform(const glm::mat4& transform) { worldMatrix = transform * GetLocalTransform(); }

        /**
         * @brief Serializes the TransformComponent.
         * @tparam Archive The type of the archive.
         * @param archive The archive to serialize to.
         */
        template <class Archive> void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("Position", Position), cereal::make_nvp("Rotation", Rotation),
                    cereal::make_nvp("Scale", Scale));
        }
    };

    /**
     * @brief Component representing a camera.
     * @ingroup scene
     */
    struct CameraComponent
    {
        SceneCamera Camera; ///< The scene camera.

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;

        /**
         * @brief Serializes the CameraComponent.
         * @tparam Archive The type of the archive.
         * @param archive The archive to serialize to.
         */
        template <class Archive> void serialize(Archive& archive) { archive(cereal::make_nvp("Camera", Camera)); }
    };

    /**
     * @brief Component representing a mesh.
     * @ingroup scene
     */
    struct MeshComponent
    {
        Ref<Mesh> mesh;        ///< The mesh reference.
        bool drawAABB = false; ///< Flag to draw the axis-aligned bounding box (AABB).

        MeshComponent()
        {
            // TEMPORAL! In the future use for example MeshComponent() : mesh(MeshFactory(PrimitiveType::MeshText))
            Ref<Model> m = Model::Load("assets/models/MissingMesh.glb");
            mesh = m->GetMeshes()[0];
        }
        MeshComponent(const MeshComponent&) = default;
        MeshComponent(Ref<Mesh> mesh) : mesh(mesh) {}

        /**
         * @brief Gets the mesh reference.
         * @return The mesh reference.
         */
        const Ref<Mesh>& GetMesh() const { return mesh; }

      private:
        friend class cereal::access;
        /**
         * @brief Serializes the MeshComponent.
         * @tparam Archive The type of the archive.
         * @param archive The archive to serialize to.
         */
        template <class Archive> void save(Archive& archive) const
        {
            archive(cereal::make_nvp("Mesh", mesh->GetUUID()));
        }

        template <class Archive> void load(Archive& archive)
        {
            UUID meshUUID;
            archive(cereal::make_nvp("Mesh", meshUUID));

            Ref<Mesh> mesh = ResourceRegistry::Get<Mesh>(meshUUID);
            this->mesh = mesh;
        }
    };

    /**
     * @brief Component representing a material.
     * @ingroup scene
     */
    struct MaterialComponent
    {
        Ref<Material> material; ///< The material reference.

        MaterialComponent()
        {
            // FIXME: The first time the Default Material is created, the UUID is not saved in the cache and each time
            // the engine is started the Default Material is created again.
            Ref<Material> m = Material::Create("Default Material");
            material = m;
        }
        MaterialComponent(const MaterialComponent&) = default;
        MaterialComponent(Ref<Material> material) : material(material) {}

      private:
        friend class cereal::access;
        /**
         * @brief Serializes the MeshComponent.
         * @tparam Archive The type of the archive.
         * @param archive The archive to serialize to.
         */
        template <class Archive> void save(Archive& archive) const
        {
            archive(cereal::make_nvp("Material", material->GetUUID()));
        }

        template <class Archive> void load(Archive& archive)
        {
            UUID materialUUID;
            archive(cereal::make_nvp("Material", materialUUID));

            Ref<Material> material = ResourceRegistry::Get<Material>(materialUUID);
            this->material = material;
        }
    };

    /**
     * @brief Component representing a light.
     * @ingroup scene
     */
    struct LightComponent
    {
        /**
         * @brief Enum representing the type of light.
         */
        enum Type
        {
            DirectionalLight = 0, ///< Directional light.
            PointLight = 1,       ///< Point light.
            SpotLight = 2         ///< Spot light.
        };

        // Align to 16 bytes(glm::vec4) instead of 12 bytes(glm::vec3) to match the std140 layout in the shader (a vec3
        // is 16 bytes in std140)
        alignas(16) glm::vec3 Color = {1.0f, 1.0f, 1.0f};      ///< The color of the light.
        alignas(16) glm::vec3 Direction = {0.0f, -1.0f, 0.0f}; ///< The direction of the light.
        alignas(16) glm::vec3 Position = {0.0f, 0.0f, 0.0f};   ///< The position of the light.

        float Range = 5.0f;       ///< The range of the light.
        float Attenuation = 1.0f; ///< The attenuation of the light.
        float Intensity = 1.0f;   ///< The intensity of the light.

        float Angle = 45.0f; ///< The angle of the light.

        int type = static_cast<int>(Type::DirectionalLight); ///< The type of the light.

        LightComponent() = default;
        LightComponent(const LightComponent&) = default;

        /**
         * @brief Serializes the LightComponent.
         * @tparam Archive The type of the archive.
         * @param archive The archive to serialize to.
         */
        template <class Archive> void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("Color", Color), cereal::make_nvp("Direction", Direction),
                    cereal::make_nvp("Position", Position), cereal::make_nvp("Range", Range),
                    cereal::make_nvp("Attenuation", Attenuation), cereal::make_nvp("Intensity", Intensity),
                    cereal::make_nvp("Angle", Angle), cereal::make_nvp("Type", type));
        }
    };

    struct RigidbodyComponent
    {
        Ref<RigidBody> m_RigidBody = nullptr;
        RigidBodyConfig cfg;
        
        RigidbodyComponent() = default;
        explicit RigidbodyComponent(TransformComponent& transform)
        {
            cfg.shapeConfig.type = CollisionShapeType::SPHERE;
            cfg.transform = transform.GetWorldTransform();
            cfg.type = RigidBodyType::Dynamic;
            m_RigidBody = std::make_shared<RigidBody>(cfg);
        }

        ~RigidbodyComponent()
        = default;

        /**
         * @brief Serializes the RigidbodyComponent.
         * @tparam Archive The type of the archive.
         * @param archive The archive to serialize to.
         */
        template <class Archive> void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("Type", cfg.type),
                    cereal::make_nvp("UseGravity", cfg.UseGravity),
                    cereal::make_nvp("Mass", cfg.shapeConfig.mass),
                    cereal::make_nvp("Velocity", cfg.Velocity),
                    cereal::make_nvp("Acceleration", cfg.Acceleration),
                    cereal::make_nvp("LinearDrag", cfg.LinearDrag),
                    cereal::make_nvp("AngularDrag", cfg.AngularDrag),
                    cereal::make_nvp("Friction", cfg.friction),
                    cereal::make_nvp("Restitution", cfg.restitution));
            if (Archive::is_loading::value)
            {
                m_RigidBody = std::make_shared<RigidBody>(cfg);
                
            }
        }

        /**
         * @brief Applies a velocity change to the rigidbody.
         * @param velocityChange The change in velocity.
         */
        void ApplyVelocityChange(const glm::vec3& velocityChange)
        {
            if (cfg.type != RigidBodyType::Static)
            {
                cfg.Velocity += velocityChange;
            }
        }
        void ApplyDrag()
        {
            if (cfg.type != RigidBodyType::Static)
            {
                cfg.Velocity *= (1.0f - cfg.LinearDrag);
            }
        }
        void ApplyAngularDrag()
        {
            if (cfg.type != RigidBodyType::Static)
            {
                // You would need a separate angular velocity value to apply angular drag, similar to velocity.
                // For now, this is a placeholder.
                // AngularVelocity *= (1.0f - AngularDrag);
            }
        }

        /**
         * @brief Applies a force to the rigidbody.
         * @param force The force to apply.
         * @param point The point at which to apply the force.
         */
        void ApplyForce(const glm::vec3& force, const glm::vec3& point = glm::vec3(0.0f))
        {
            if (m_RigidBody)
                m_RigidBody->ApplyForce(force, point);
        }

        /**
         * @brief Applies an impulse to the rigidbody.
         * @param impulse The impulse to apply.
         * @param point The point at which to apply the impulse.
         */
        void ApplyImpulse(const glm::vec3& impulse, const glm::vec3& point = glm::vec3(0.0f))
        {
            if (m_RigidBody)
                m_RigidBody->ApplyImpulse(impulse, point);
        }
        void ApplyShape(btCollisionShape* shape, const glm::vec3 position,
                        const glm::quat rotation, const glm::vec3 size)
        {
            if (m_RigidBody)
                m_RigidBody->ApplyShape(shape, position, rotation, size);
        }

        void SetVelocity(const glm::vec3& velocity)
        {
            if (m_RigidBody)
            {
                m_RigidBody->SetVelocity(velocity);
                cfg.Velocity = velocity;
            }
        }

        void SetAngularVelocity(const glm::vec3& angularVelocity)
        {
            if (m_RigidBody)
            {
                m_RigidBody->SetAngularVelocity(angularVelocity);
            }
        }

        glm::vec3 GetVelocity() const
        {
            if (m_RigidBody)
                return m_RigidBody->GetVelocity();
            return cfg.Velocity;
        }

        glm::vec3 GetAngularVelocity() const 
        {
            if (m_RigidBody)
                return m_RigidBody->GetAngularVelocity();
            return glm::vec3(0.0f);
        }

        void AddVelocity(const glm::vec3& velocityChange)
        {
            if (m_RigidBody)
            {
                m_RigidBody->AddVelocity(velocityChange);
            }
        }
    };

   
    enum class ColliderShape
    {
        Box = 0,
        Sphere,
        Capsule,
        Cylinder
    };

    struct ColliderComponent
    {
        ColliderShape Shape = ColliderShape::Box; // Tipo de collider
        glm::vec3 Offset = {0.0f, 0.0f, 0.0f};    // Offset del collider

        // Propiedades específicas según la Shape
        glm::vec3 Size = {1.0f, 1.0f, 1.0f}; // Para Box
        float Radius = 0.5f;                 // Para Sphere, Capsule y Cylinder
        float Height = 1.0f;                 // Para Capsule y Cylinder

        bool IsTrigger = false; // Es un trigger
        float Mass = 0.0f;      // Masa del collider
        int MaterialIndex = 0;  // Índice del material

        Ref<Collider> m_Collider = nullptr; // Referencia al Collider

        ColliderComponent() = default;

        explicit ColliderComponent(TransformComponent& transform) { UpdateCollider(transform); }

        ~ColliderComponent() = default;

        void UpdateCollider(TransformComponent& transform)
        {
            // Configurar el Collider según la forma seleccionada
            CollisionShapeConfig config;
            config.isTrigger = IsTrigger;
            config.mass = Mass;

            switch (Shape)
            {
            case ColliderShape::Box:
                config.type = CollisionShapeType::BOX;
                config.size = Size;
                break;
            case ColliderShape::Sphere:
                config.type = CollisionShapeType::SPHERE;
                config.radius = Radius;
                break;
            case ColliderShape::Capsule:
                config.type = CollisionShapeType::CAPSULE;
                config.radius = Radius;
                config.height = Height;
                break;
            case ColliderShape::Cylinder:
                config.type = CollisionShapeType::CYLINDER;
                config.radius = Radius;
                config.height = Height;
                break;
            }

            glm::vec3 position = glm::vec3(transform.GetWorldTransform()[3]) + Offset;
            glm::quat rotation = glm::quat(glm::vec3(0.0f));
            glm::vec3 scale = glm::vec3(1.0f);

            m_Collider = std::make_shared<Collider>(config, position, rotation, scale);
        }

        /**
         * @brief Serializes the ColliderComponent.
         */
        template <class Archive> void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("Shape", Shape), cereal::make_nvp("Size", Size),
                    cereal::make_nvp("Offset", Offset), cereal::make_nvp("Radius", Radius),
                    cereal::make_nvp("Height", Height), cereal::make_nvp("IsTrigger", IsTrigger),
                    cereal::make_nvp("Mass", Mass), cereal::make_nvp("MaterialIndex", MaterialIndex));

            if (Archive::is_loading::value)
            {
                TransformComponent dummyTransform; // Necesario para crear el Collider
                UpdateCollider(dummyTransform);
            }
        }
    };



    struct SphereColliderComponent
    {
        glm::vec3 Center = {0.0f, 0.0f, 0.0f}; // center of the collider
        float Radius = 0.5f;                   // radius of the collider
        bool IsTrigger = false;                // is the collider a trigger
        bool ProvidesContacts = false;         // provides contacts
        int MaterialIndex = 0;                 // index for the material dropdown

        SphereColliderComponent() = default;
        SphereColliderComponent(const glm::vec3& center, float radius, bool isTrigger, bool providesContacts,
                                int materialIndex = 0)
            : Center(center), Radius(radius), IsTrigger(isTrigger), ProvidesContacts(providesContacts),
              MaterialIndex(materialIndex)
        {
        }
    };
    struct CapsuleColliderComponent
    {
        glm::vec3 Center = {0.0f, 0.0f, 0.0f}; // center of the collider
        float Radius = 0.5f;                   // radius of the collider
        float Height = 1.0f;                   // height of the collider
        int DirectionIndex = 1;                // direction index for the collider (0: X-Axis, 1: Y-Axis, 2: Z-Axis)
        bool IsTrigger = false;                // is the collider a trigger
        bool ProvidesContacts = false;         // provides contacts
        int MaterialIndex = 0;                 // index for the material dropdown

        CapsuleColliderComponent() = default;
        CapsuleColliderComponent(const glm::vec3& center, float radius, float height, int directionIndex,
                                 bool isTrigger, bool providesContacts, int materialIndex = 0)
            : Center(center), Radius(radius), Height(height), DirectionIndex(directionIndex), IsTrigger(isTrigger),
              ProvidesContacts(providesContacts), MaterialIndex(materialIndex)
        {
        }
    };
    struct CylinderColliderComponent
    {
        glm::vec3 Center = {0.0f, 0.0f, 0.0f}; // center of the collider
        float Radius = 0.5f;                   // radius of the collider
        float Height = 1.0f;                   // height of the collider
        int DirectionIndex = 1;                // direction index for the collider (0: X-Axis, 1: Y-Axis, 2: Z-Axis)
        bool IsTrigger = false;                // is the collider a trigger
        bool ProvidesContacts = false;         // provides contacts
        int MaterialIndex = 0;                 // index for the material dropdown

        CylinderColliderComponent() = default;
        CylinderColliderComponent(const glm::vec3& center, float radius, float height, int directionIndex,
                                  bool isTrigger, bool providesContacts, int materialIndex = 0)
            : Center(center), Radius(radius), Height(height), DirectionIndex(directionIndex), IsTrigger(isTrigger),
              ProvidesContacts(providesContacts), MaterialIndex(materialIndex)
        {
        }
    };
    struct PlaneColliderComponent
    {
        glm::vec3 Size = {1.0f, 1.0f, 1.0f};   // Size of the collider
        glm::vec3 Offset = {0.0f, 0.0f, 0.0f}; // offset of the collider
        bool IsTrigger = false;                // is the collider a trigger

        PlaneColliderComponent() = default;
        PlaneColliderComponent(const glm::vec3& size, const glm::vec3& offset, bool isTrigger)
            : Size(size), Offset(offset), IsTrigger(isTrigger)
        {
        }
    };
    struct MeshColliderComponent
    {
        glm::vec3 Size = {1.0f, 1.0f, 1.0f};   // Size of the collider
        glm::vec3 Offset = {0.0f, 0.0f, 0.0f}; // offset of the collider
        bool IsTrigger = false;                // is the collider a trigger
        bool ProvidesContacts = false;         // provides contacts
        int CookingOptionsIndex = 1;           // index for the cooking options dropdown
        int MaterialIndex = 0;                 // index for the material dropdown
        int MeshIndex = 0;                     // index for the mesh dropdown

        MeshColliderComponent() = default;
        MeshColliderComponent(const glm::vec3& size, const glm::vec3& offset, bool isTrigger, bool providesContacts,
                              int cookingOptionsIndex = 1, int materialIndex = 0, int meshIndex = 0)
            : Size(size), Offset(offset), IsTrigger(isTrigger), ProvidesContacts(providesContacts),
              CookingOptionsIndex(cookingOptionsIndex), MaterialIndex(materialIndex), MeshIndex(meshIndex)
        {
        }
    };

    // joint
    struct FixedJointComponent
    {
        char ConnectedBody[128] = "";    // Identifier for connected body
        float BreakForce = FLT_MAX;      // Force threshold for breaking the joint
        float BreakTorque = FLT_MAX;     // Torque threshold for breaking the joint
        bool EnableCollision = false;    // Whether to enable collision
        bool EnablePreprocessing = true; // Whether to enable preprocessing
        float MassScale = 1.0f;          // Scale for this object's mass
        float ConnectedMassScale = 1.0f; // Scale for the connected body's mass

        FixedJointComponent() = default;
    };

    struct SpringJointComponent
    {
        char ConnectedBody[128] = "";
        glm::vec3 Anchor = {0.0f, 0.0f, 0.0f};
        bool AutoConfigureConnectedAnchor = true;
        glm::vec3 ConnectedAnchor = {0.0f, 0.0f, 0.0f};
        float Spring = 0.0f;
        float Damper = 0.0f;
        float MinDistance = 0.0f;
        float MaxDistance = 0.0f;
        float Tolerance = 0.025f;
        float BreakForce = FLT_MAX;
        float BreakTorque = FLT_MAX;
        bool EnableCollision = false;
        bool EnablePreprocessing = true;
        float MassScale = 1.0f;
        float ConnectedMassScale = 1.0f;

        SpringJointComponent() = default;
    };

    struct DistanceJoint2DComponent
    {
        char ConnectedRigidbody[128] = "";                 // Identifier for connected rigidbody
        bool EnableCollision = false;                      // Whether to enable collision
        bool AutoConfigureConnectedAnchor = true;          // Auto configure the connected anchor
        glm::vec2 Anchor = glm::vec2(0.0f, 0.0f);          // Anchor position
        glm::vec2 ConnectedAnchor = glm::vec2(0.0f, 0.0f); // Connected anchor position
        bool AutoConfigureDistance = true;                 // Auto configure the distance
        float Distance = 0.0f;                             // The distance between the two anchor points
        bool MaxDistanceOnly = false;                      // Whether to enforce only the maximum distance
        int BreakAction = 0;                               // Break action when force/torque exceeds limit
        float BreakForce = FLT_MAX;                        // Force threshold for breaking the joint

        DistanceJoint2DComponent() = default;
    };

    struct SliderJoint2DComponent
    {
        char ConnectedRigidbody[128] = "";                 // Identifier for connected rigidbody
        bool EnableCollision = false;                      // Whether to enable collision
        glm::vec2 Anchor = glm::vec2(0.0f, 0.0f);          // Anchor position
        glm::vec2 ConnectedAnchor = glm::vec2(0.0f, 0.0f); // Connected anchor position
        float Angle = 0.0f;                                // Angle of the slider joint
        bool UseMotor = false;                             // Whether to use motor
        float MotorSpeed = 0.0f;                           // Motor speed
        float MaxMotorForce = 0.0f;                        // Maximum motor force
        bool UseLimits = false;                            // Whether to use translation limits
        float MinTranslation = 0.0f;                       // Minimum translation limit
        float MaxTranslation = 0.0f;                       // Maximum translation limit
        int BreakAction = 0;                               // Break action when force/torque exceeds limit
        float BreakForce = FLT_MAX;                        // Force threshold for breaking the joint
        float BreakTorque = FLT_MAX;                       // Torque threshold for breaking the joint

        SliderJoint2DComponent() = default;
    };

   } // namespace Coffee

    /** @} */