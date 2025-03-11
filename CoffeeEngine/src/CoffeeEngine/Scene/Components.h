/**
* @defgroup scene Scene
* @{
*/

#pragma once

#include "CoffeeEngine/Core/Base.h"
#include "CoffeeEngine/IO/ResourceLoader.h"
#include "CoffeeEngine/IO/ResourceRegistry.h"
#include "CoffeeEngine/Physics/Collider.h"
#include "CoffeeEngine/Animation/AnimationSystem.h"
#include "CoffeeEngine/Physics/RigidBody.h"
#include "CoffeeEngine/Renderer/Material.h"
#include "CoffeeEngine/Renderer/Mesh.h"
#include "CoffeeEngine/Scene/SceneCamera.h"
#include "CoffeeEngine/Scripting/Script.h"
#include "CoffeeEngine/Scripting/ScriptManager.h"
#include <cereal/cereal.hpp>
#include <cereal/access.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <CoffeeEngine/Physics/CollisionCallback.h>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

#include "CoffeeEngine/Project/Project.h"

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

// FIXME : DONT MOVE THIS INCLUDE
#include "CoffeeEngine/Audio/Audio.h"


namespace Coffee {
    /**
    * @brief Component representing a tag.
    * @ingroup scene
    */
    struct TagComponent
    {
        std::string Tag; ///< The tag string.

        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& tag)
            : Tag(tag) {}

        /**
        * @brief Serializes the TagComponent.
        * @tparam Archive The type of the archive.
        * @param archive The archive to serialize to.
        */
        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("Tag", Tag));
        }
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
        glm::vec3 Position = { 0.0f, 0.0f, 0.0f }; ///< The position vector.
        glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f }; ///< The rotation vector.
        glm::vec3 Scale = { 1.0f, 1.0f, 1.0f }; ///< The scale vector.

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::vec3& position)
            : Position(position) {}

        /**
        * @brief Gets the local transformation matrix.
        * @return The local transformation matrix.
        */
        glm::mat4 GetLocalTransform() const
        {
            glm::mat4 rotation = glm::toMat4(glm::quat(glm::radians(Rotation)));

            return glm::translate(glm::mat4(1.0f), Position)
                    * rotation
                    * glm::scale(glm::mat4(1.0f), Scale);
        }

        /**
        * @brief Sets the local transformation matrix.
        * @param transform The transformation matrix to set.
        */
        void SetLocalTransform(const glm::mat4& transform) //TODO: Improve this function, this way is ugly and glm::decompose is from gtx (is supposed to not be very stable)
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
        const glm::mat4& GetWorldTransform() const
        {
            return worldMatrix;
        }

        /**
        * @brief Sets the world transformation matrix.
        * @param transform The transformation matrix to set.
        */
        void SetWorldTransform(const glm::mat4& transform)
        {
            worldMatrix = transform * GetLocalTransform();
        }

        /**
        * @brief Serializes the TransformComponent.
        * @tparam Archive The type of the archive.
        * @param archive The archive to serialize to.
        */
        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("Position", Position), cereal::make_nvp("Rotation", Rotation), cereal::make_nvp("Scale", Scale));
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
        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("Camera", Camera));
        }
    };

    /**
    * @brief Component representing an animator.
    * @ingroup scene
    */
    struct AnimatorComponent
    {
        AnimatorComponent() = default;

        /**
        * @brief Copy constructor for AnimatorComponent.
        * @param other The other AnimatorComponent to copy from.
        */
        AnimatorComponent(const AnimatorComponent& other)
        : IsBlending(other.IsBlending),
        CurrentAnimation(other.CurrentAnimation),
        NextAnimation(other.NextAnimation),
        AnimationTime(other.AnimationTime),
        NextAnimationTime(other.NextAnimationTime),
        BlendTime(other.BlendTime),
        BlendDuration(other.BlendDuration),
        BlendThreshold(other.BlendThreshold),
        AnimationSpeed(other.AnimationSpeed),
        JointMatrices(other.JointMatrices),
        m_Skeleton(other.m_Skeleton),
        m_AnimationController(other.m_AnimationController),
        m_AnimationSystem(other.m_AnimationSystem),
        modelUUID(other.modelUUID),
        animatorUUID(other.animatorUUID)
        {
        }

        /**
        * @brief Constructs an AnimatorComponent with the given skeleton, animation controller, and animation system.
        * @param skeleton The skeleton reference.
        * @param animationController The animation controller reference.
        * @param animationSystem The animation system reference.
        */
        AnimatorComponent(Ref<Skeleton> skeleton, Ref<AnimationController> animationController, Ref<AnimationSystem> animationSystem)
        : m_Skeleton(std::move(skeleton)), m_AnimationController(std::move(animationController)), m_AnimationSystem(std::move(animationSystem))
        {
            m_BlendJob.layers = ozz::make_span(m_BlendLayers);
            JointMatrices = m_Skeleton->GetJointMatrices();
        }

        /**
        * @brief Gets the skeleton reference.
        * @return The skeleton reference.
        */
        Ref<Skeleton> GetSkeleton() const { return m_Skeleton; }

        /**
        * @brief Gets the animation controller reference.
        * @return The animation controller reference.
        */
        Ref<AnimationController> GetAnimationController() const { return m_AnimationController; }

        /**
        * @brief Gets the animation system reference.
        * @return The animation system reference.
        */
        Ref<AnimationSystem> GetAnimationSystem() const { return m_AnimationSystem; }

        /**
        * @brief Gets the sampling job context.
        * @return The sampling job context.
        */
        ozz::animation::SamplingJob::Context& GetContext() { return m_Context; }

        /**
        * @brief Gets the blend layers.
        * @return The blend layers.
        */
        ozz::animation::BlendingJob::Layer* GetBlendLayers() { return m_BlendLayers; }

        /**
        * @brief Gets the blending job.
        * @return The blending job.
        */
        ozz::animation::BlendingJob& GetBlendJob() { return m_BlendJob; }


        void SetCurrentAnimation(int index) { m_AnimationSystem->SetCurrentAnimation(index, this);}

        /**
        * @brief Serializes the AnimatorComponent.
        * @tparam Archive The type of the archive.
        * @param archive The archive to serialize to.
        */
        template<class Archive>
        void save(Archive& archive) const
        {
            archive(cereal::make_nvp("CurrentAnimation", CurrentAnimation),
                    cereal::make_nvp("BlendDuration", BlendDuration),
                    cereal::make_nvp("BlendThreshold", BlendThreshold),
                    cereal::make_nvp("AnimationSpeed", AnimationSpeed),
                    cereal::make_nvp("ModelUUID", modelUUID),
                    cereal::make_nvp("AnimatorUUID", animatorUUID));
        }

        /**
        * @brief Deserializes the AnimatorComponent.
        * @tparam Archive The type of the archive.
        * @param archive The archive to deserialize from.
        */
        template<class Archive>
        void load(Archive& archive)
        {
            archive(cereal::make_nvp("CurrentAnimation", CurrentAnimation),
                    cereal::make_nvp("BlendDuration", BlendDuration),
                    cereal::make_nvp("BlendThreshold", BlendThreshold),
                    cereal::make_nvp("AnimationSpeed", AnimationSpeed),
                    cereal::make_nvp("ModelUUID", modelUUID),
                    cereal::make_nvp("AnimatorUUID", animatorUUID));
        }

    public:
        bool IsBlending = false; ///< Indicates if the animation is blending.
        unsigned int CurrentAnimation = 0; ///< The current animation index.
        unsigned int NextAnimation = 0; ///< The next animation index.
        float AnimationTime = 0.f; ///< The current animation time.
        float NextAnimationTime = 0.f; ///< The next animation time.
        float BlendTime = 0.f; ///< The current blend time.
        float BlendDuration = 0.25f; ///< The duration of the blend.
        float BlendThreshold = 0.8; ///< The blend threshold.
        float AnimationSpeed = 1.0f; ///< The speed of the animation.

        std::vector<glm::mat4> JointMatrices; ///< The joint matrices.
        UUID modelUUID; ///< The UUID of the model.
        UUID animatorUUID; ///< The UUID of the animator.

        Ref<Skeleton> m_Skeleton; ///< The skeleton reference.
        Ref<AnimationController> m_AnimationController; ///< The animation controller reference.
        Ref<AnimationSystem> m_AnimationSystem; ///< The animation system reference.

        ozz::animation::SamplingJob::Context m_Context; ///< The sampling job context.
        ozz::animation::BlendingJob::Layer m_BlendLayers[2]; ///< The blend layers.
        ozz::animation::BlendingJob m_BlendJob; ///< The blending job.
    };

    /**
    * @brief Component representing a mesh.
    * @ingroup scene
    */
    struct MeshComponent
    {
        Ref<Mesh> mesh; ///< The mesh reference.
        bool drawAABB = false; ///< Flag to draw the axis-aligned bounding box (AABB).

        AnimatorComponent* animator = nullptr; ///< The animator component.
        UUID animatorUUID = 0; ///< The UUID of the animator.

        MeshComponent()
        {
        }
        MeshComponent(const MeshComponent&) = default;
        MeshComponent(Ref<Mesh> mesh)
            : mesh(mesh) {}

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
        template<class Archive>
        void save(Archive& archive) const
        {
            archive(cereal::make_nvp("Mesh", mesh->GetUUID()),
                    cereal::make_nvp("AnimatorUUID", animatorUUID));

            if (animator && animatorUUID != 0)
                animator->animatorUUID = animatorUUID;
        }

        template<class Archive>
        void load(Archive& archive)
        {
            UUID meshUUID;
            archive(cereal::make_nvp("Mesh", meshUUID),
                    cereal::make_nvp("AnimatorUUID", animatorUUID));

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
            // FIXME: The first time the Default Material is created, the UUID is not saved in the cache and each time the engine is started the Default Material is created again.
            Ref<Material> m = Material::Create("Default Material");
            material = m;
        }
        MaterialComponent(const MaterialComponent&) = default;
        MaterialComponent(Ref<Material> material)
            : material(material) {}

        private:
            friend class cereal::access;
        /**
        * @brief Serializes the MeshComponent.
        * @tparam Archive The type of the archive.
        * @param archive The archive to serialize to.
        */
        template<class Archive>
        void save(Archive& archive) const
        {
            archive(cereal::make_nvp("Material", material->GetUUID()));
        }

        template<class Archive>
        void load(Archive& archive)
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
            PointLight = 1, ///< Point light.
            SpotLight = 2 ///< Spot light.
        };

        // Align to 16 bytes(glm::vec4) instead of 12 bytes(glm::vec3) to match the std140 layout in the shader (a vec3 is 16 bytes in std140)
        alignas(16) glm::vec3 Color = {1.0f, 1.0f, 1.0f}; ///< The color of the light.
        alignas(16) glm::vec3 Direction = {0.0f, -1.0f, 0.0f}; ///< The direction of the light.
        alignas(16) glm::vec3 Position = {0.0f, 0.0f, 0.0f}; ///< The position of the light.

        float Range = 5.0f; ///< The range of the light.
        float Attenuation = 1.0f; ///< The attenuation of the light.
        float Intensity = 1.0f; ///< The intensity of the light.

        float Angle = 45.0f; ///< The angle of the light.

        int type = static_cast<int>(Type::DirectionalLight); ///< The type of the light.

        LightComponent() = default;
        LightComponent(const LightComponent&) = default;

        /**
        * @brief Serializes the LightComponent.
        * @tparam Archive The type of the archive.
        * @param archive The archive to serialize to.
        */
        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("Color", Color), cereal::make_nvp("Direction", Direction), cereal::make_nvp("Position", Position), cereal::make_nvp("Range", Range), cereal::make_nvp("Attenuation", Attenuation), cereal::make_nvp("Intensity", Intensity), cereal::make_nvp("Angle", Angle), cereal::make_nvp("Type", type));
        }
    };

    struct AudioSourceComponent
    {
        uint64_t gameObjectID = -1; ///< The object ID.
        Ref<Audio::AudioBank> audioBank; ///< The audio bank.
        std::string audioBankName; ///< The name of the audio bank.
        std::string eventName; ///< The name of the event.
        float volume = 1.f; ///< The volume of the audio source.
        bool mute = false; ///< True if the audio source is muted.
        bool playOnAwake = false; ///< True if the audio source should play automatically.
        glm::mat4 transform; ///< The transform of the audio source.
        bool isPlaying = false; ///< True if the audio source is playing.
        bool isPaused = false; ///< True if the audio source is paused.
        bool toDelete = false; ///< True if the audio source should be deleted.

        AudioSourceComponent() = default;

        AudioSourceComponent(const AudioSourceComponent& other)
        {
            *this = other;
        }

        AudioSourceComponent& operator=(const AudioSourceComponent& other)
        {
            if (this != &other)
            {
                gameObjectID = other.gameObjectID;
                audioBank = other.audioBank;
                audioBankName = other.audioBankName;
                eventName = other.eventName;
                volume = other.volume;
                mute = other.mute;
                playOnAwake = other.playOnAwake;
                transform = other.transform;
                isPlaying = other.isPlaying;
                isPaused = other.isPaused;
                toDelete = other.toDelete;

                if (!toDelete)
                {
                    Audio::RegisterAudioSourceComponent(*this);
                    AudioZone::RegisterObject(gameObjectID, transform[3]);
                }
            }
            return *this;
        }

        void SetVolume(float volumen)
        {
            if (volumen > 1)
            {
                volumen = 1;
            }
            else if (volumen < 0)
            {
                volumen = 0;
            }
            volume = volumen;
            Audio::SetVolume(this->gameObjectID, this->volume);
        }

        void Play() { Audio::PlayEvent(*this); }
        void Stop() { Audio::StopEvent(*this); }


        template<class Archive>
        void save(Archive& archive) const
        {
            archive(cereal::make_nvp("GameObjectID", gameObjectID),
                    cereal::make_nvp("AudioBank", audioBank),
                    cereal::make_nvp("AudioBankName", audioBankName),
                    cereal::make_nvp("EventName", eventName),
                    cereal::make_nvp("Volume", volume),
                    cereal::make_nvp("Mute", mute),
                    cereal::make_nvp("PlayOnAwake", playOnAwake),
                    cereal::make_nvp("Transform", transform)
            );
        }

        template<class Archive>
        void load(Archive& archive)
        {
            archive(cereal::make_nvp("GameObjectID", gameObjectID),
                    cereal::make_nvp("AudioBank", audioBank),
                    cereal::make_nvp("AudioBankName", audioBankName),
                    cereal::make_nvp("EventName", eventName),
                    cereal::make_nvp("Volume", volume),
                    cereal::make_nvp("Mute", mute),
                    cereal::make_nvp("PlayOnAwake", playOnAwake),
                    cereal::make_nvp("Transform", transform)
            );
        }
    };

    struct AudioListenerComponent
    {
        uint64_t gameObjectID = -1; ///< The object ID.
        glm::mat4 transform; ///< The transform of the audio listener.
        bool toDelete = false; ///< True if the audio listener should be deleted.

        AudioListenerComponent() = default;

        AudioListenerComponent(const AudioListenerComponent& other)
        {
            *this = other;
        }

        AudioListenerComponent& operator=(const AudioListenerComponent& other)
        {
            if (this != &other)
            {
                gameObjectID = other.gameObjectID;
                transform = other.transform;
                toDelete = other.toDelete;

                if (!toDelete)
                    Audio::RegisterAudioListenerComponent(*this);
            }
            return *this;
        }

        template<class Archive>
        void save(Archive& archive) const
        {
            archive(cereal::make_nvp("GameObjectID", gameObjectID),
                    cereal::make_nvp("Transform", transform)
            );
        }

        template<class Archive>
        void load(Archive& archive)
        {
            archive(cereal::make_nvp("GameObjectID", gameObjectID),
                    cereal::make_nvp("Transform", transform)
            );
        }
    };

    struct AudioZoneComponent
    {
        uint64_t zoneID = -1; ///< The zone ID.
        std::string audioBusName; ///< The name of the audio bus.
        glm::vec3 position = { 0.f, 0.f, 0.f }; ///< The position of the audio zone.
        float radius = 1.f; ///< The radius of the audio zone.

        AudioZoneComponent() = default;

        AudioZoneComponent(const AudioZoneComponent& other)
        {
            *this = other;
        }

        AudioZoneComponent& operator=(const AudioZoneComponent& other)
        {
            if (this != &other)
            {
                zoneID = other.zoneID;
                audioBusName = other.audioBusName;
                position = other.position;
                radius = other.radius;

                AudioZone::CreateZone(*this);
            }
            return *this;
        }

        template<class Archive>
        void save(Archive& archive) const
        {
            archive(cereal::make_nvp("ZoneID", zoneID),
                    cereal::make_nvp("AudioBusName", audioBusName),
                    cereal::make_nvp("Position", position),
                    cereal::make_nvp("Radius", radius)
            );
        }

        template<class Archive>
        void load(Archive& archive)
        {
            archive(cereal::make_nvp("ZoneID", zoneID),
                    cereal::make_nvp("AudioBusName", audioBusName),
                    cereal::make_nvp("Position", position),
                    cereal::make_nvp("Radius", radius)
            );
        }
    };
    
    struct ScriptComponent
    {
        Ref<Script> script;

        ScriptComponent() = default;
        ScriptComponent(const std::filesystem::path& path, ScriptingLanguage language)
        {
            switch (language)
            {
                using enum ScriptingLanguage;
            case Lua:
                script = ScriptManager::CreateScript(path, language);
                break;
            case cSharp:
                break;
            }
        }

        /**
        * @brief Serializes the ScriptComponent.
        *
        * This function serializes the ScriptComponent by storing the script path and language.
        * Note: Currently, this system only supports Lua scripting language.
        *
        * @tparam Archive The type of the archive.
        * @param archive The archive to serialize to.
        */
        template<class Archive>
        void save(Archive& archive) const
        {
            std::filesystem::path relativePath;
            if (Project::GetActive())
            {
                relativePath = std::filesystem::relative(script->GetPath(), Project::GetActive()->GetProjectDirectory());
            }
            else
            {
                relativePath = script->GetPath();
                COFFEE_CORE_ERROR("ScriptComponent::save: Project is not active, script path is not relative to the project directory!");
            }
            archive(cereal::make_nvp("ScriptPath", relativePath.string()), cereal::make_nvp("Language", ScriptingLanguage::Lua));
        }

        template<class Archive>
        void load(Archive& archive)
        {
            std::string relativePath;
            ScriptingLanguage language;

            archive(cereal::make_nvp("ScriptPath", relativePath), cereal::make_nvp("Language", language));

            if (!relativePath.empty())
            {
                std::filesystem::path scriptPath;
                if (Project::GetActive())
                {
                    scriptPath = Project::GetActive()->GetProjectDirectory() / relativePath;
                }
                else
                {
                    scriptPath = relativePath;
                    COFFEE_CORE_ERROR("ScriptComponent::load: Project is not active, script path is not relative to the project directory!");
                }

                switch (language)
                {
                    using enum ScriptingLanguage;
                case Lua:
                    script = ScriptManager::CreateScript(scriptPath, language);
                    break;
                case cSharp:
                    // Handle cSharp script loading if needed
                    break;
                }
            }
        }
/* 
        static void OnConstruct(entt::registry& registry, entt::entity entity)
        {
            auto& scriptComponent = registry.get<ScriptComponent<DerivedScript>>(entity);

            if(Editor is in runtime)
            {
                ScriptManager::ExecuteScript(scriptComponent.script);
                script.OnScenetreeEntered();
            }
        } */


    };

    struct RigidbodyComponent {
        public:
            Ref<RigidBody> rb;
            CollisionCallback callback;

            RigidbodyComponent() = default;
            RigidbodyComponent(const RigidbodyComponent&) = default;
            RigidbodyComponent(const RigidBody::Properties& props, Ref<Collider> collider) {
                rb = RigidBody::Create(props, collider);
            }

            ~RigidbodyComponent() {
                rb.reset();
            }

        private:
            friend class cereal::access;

            template<class Archive>
            void save(Archive& archive) const {
                if (rb) {
                    archive(
                        cereal::make_nvp("RigidBody", true),
                        cereal::make_nvp("RigidBodyData", rb)
                    );
                } else {
                    archive(cereal::make_nvp("RigidBody", false));
                }
            }

            template<class Archive>
            void load(Archive& archive) {
                bool hasRigidBody;
                archive(cereal::make_nvp("RigidBody", hasRigidBody));
                
                if (hasRigidBody) {
                    archive(cereal::make_nvp("RigidBodyData", rb));
                }
            }
    };

    struct ColliderComponent {
        Ref<Collider> collider;
        bool isTrigger = false;
        CollisionCallback callback;

        ColliderComponent() = default;
        ColliderComponent(const ColliderComponent&) = default;

        ColliderComponent(const Ref<Collider>& inCollider, bool inIsTrigger = false)
            : collider(inCollider), isTrigger(inIsTrigger) {}

    private:
        friend class cereal::access;

        template<class Archive>
        void save(Archive& archive) const {
            archive(
                CEREAL_NVP(collider),
                CEREAL_NVP(isTrigger)
            );
        }

        template<class Archive>
        void load(Archive& archive) {
            archive(
                CEREAL_NVP(collider),
                CEREAL_NVP(isTrigger)
            );
        }
    };
}

/** @} */