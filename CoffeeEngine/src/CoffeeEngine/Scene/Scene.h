#pragma once

#include "CoffeeEngine/Core/DataStructures/Octree.h"
#include "CoffeeEngine/Events/Event.h"
#include "CoffeeEngine/Physics/PhysicsWorld.h"
#include "CoffeeEngine/Renderer/EditorCamera.h"
#include "CoffeeEngine/Scene/SceneTree.h"
#include "CoffeeEngine/Scene/Components.h"
#include "entt/entity/fwd.hpp"

#include <entt/entt.hpp>
#include <filesystem>
#include <string>
#include <vector>

namespace Coffee {
    class AnimationSystem;

    /**
     * @defgroup scene Scene
     * @{
     */

    class AnimatorComponent;
    class Entity;
    class Model;

    /**
     * @brief Class representing a scene.
     * @ingroup scene
     */
    class Scene
    {
    public:
        /**
         * @brief Constructor for Scene.
         */
        Scene();

        /**
         * @brief Default destructor.
         */
        ~Scene() = default;

        //Scene(Ref<Scene> other);

        /**
         * @brief Create an entity in the scene.
         * @param name The name of the entity.
         * @return The created entity.
         */
        Entity CreateEntity(const std::string& name = std::string());

        /**
         * @brief Destroy an entity in the scene.
         * @param entity The entity to destroy.
         */
        void DestroyEntity(Entity entity);

        Entity GetEntityByName(const std::string& name);

        std::vector<Entity> GetAllEntities();

        template<typename... Components>
        auto GetAllEntitiesWithComponents()
        {
            return m_Registry.view<Components...>();
        }

        /**
         * @brief Initialize the scene.
         */
        void OnInitEditor();
        void OnInitRuntime();

        /**
         * @brief Update the scene in editor mode.
         * @param camera The editor camera.
         * @param dt The delta time.
         */
        void OnUpdateEditor(EditorCamera& camera, float dt);

        /**
         * @brief Update the scene in runtime mode.
         * @param dt The delta time.
         */
        void OnUpdateRuntime(float dt);

        /**
         * @brief Handle an event in the scene.
         * @param e The event.
         */
        void OnEvent(Event& e);

        /**
         * @brief Exit the scene.
         */
        void OnExitEditor();
        void OnExitRuntime();

        /**
         * @brief Load a scene from a file.
         * @param path The path to the file.
         * @return The loaded scene.
         */
        static Ref<Scene> Load(const std::filesystem::path& path);

        /**
         * @brief Save a scene to a file.
         * @param path The path to the file.
         * @param scene The scene to save.
         */
        static void Save(const std::filesystem::path& path, Ref<Scene> scene);

        /**
         * @brief Update the positions of the audio components.
         */
        void UpdateAudioComponentsPositions();

        const std::filesystem::path& GetFilePath() { return m_FilePath; }

        /**
         * @brief Gets the animation system.
         * @return A reference to the animation system.
         */
        static const Ref<AnimationSystem> GetAnimationSystem() { return m_AnimationSystem; }

        /**
         * @brief Assigns animators to meshes.
         * @param animators The vector of animator components.
         */
        void AssignAnimatorsToMeshes(const std::vector<AnimatorComponent*> animators);

        /**
         * @brief Gets the physics world.
         * @return A reference to the physics world.
         */
        const PhysicsWorld& GetPhysicsWorld() const { return m_PhysicsWorld; }

    private:
        entt::registry m_Registry;
        Scope<SceneTree> m_SceneTree;
        Octree<Ref<Mesh>> m_Octree;
        PhysicsWorld m_PhysicsWorld;

        // Temporal: Scenes should be Resources and the Base Resource class already has a path variable.
        std::filesystem::path m_FilePath;

        static Ref<AnimationSystem> m_AnimationSystem; ///< The animation system.

        friend class Entity;
        friend class SceneTree;
        friend class SceneTreePanel;
        friend class CollisionSystem;

        // TODO REMOVE PLEASE, THIS IS ONLY TO TEST THE OCTREE!!!!
        friend class EditorLayer;
    };

    /**
     * @brief Add a model to the scene tree.
     * @param scene The scene.
     * @param model The model to add.
     */
    void AddModelToTheSceneTree(Scene* scene, Ref<Model> model, AnimatorComponent* animatorComponent = nullptr);

    /** @} */ // end of scene group
}