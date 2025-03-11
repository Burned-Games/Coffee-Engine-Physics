#include "CollisionSystem.h"
#include "CoffeeEngine/Scene/Components.h"
#include <BulletCollision/NarrowPhaseCollision/btPersistentManifold.h>

namespace Coffee {

    Scene* CollisionSystem::s_Scene = nullptr;
    std::unordered_set<std::pair<btCollisionObject*, btCollisionObject*>, PairHash> CollisionSystem::s_ActiveCollisions;
    std::unordered_map<btCollisionObject*, Entity> CollisionSystem::s_CollisionObjectToEntity;

    void CollisionSystem::Initialize(Scene* scene)
    {
        s_Scene = scene;
    }

    void CollisionSystem::RegisterCollider(Entity entity, btCollisionObject* collisionObject) {
        if (!collisionObject) return;

        // Store the entity handle in the collision object's user pointer
        collisionObject->setUserPointer(reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<entt::entity>(entity))));

        // Store mapping from collision object to entity
        s_CollisionObjectToEntity[collisionObject] = entity;
    }

    void CollisionSystem::RegisterRigidbody(Entity entity, btRigidBody* rigidBody) {
        if (!rigidBody) return;

        // Store the entity handle in the rigidbody's user pointer
        rigidBody->setUserPointer(reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<entt::entity>(entity))));

        // Store mapping from collision object to entity
        s_CollisionObjectToEntity[rigidBody] = entity;
    }

    void CollisionSystem::UnregisterCollisionObject(btCollisionObject* collisionObject) {
        if (!collisionObject) return;

        // Remove from active collisions
        auto it = s_ActiveCollisions.begin();
        while (it != s_ActiveCollisions.end()) {
            if (it->first == collisionObject || it->second == collisionObject) {
                it = s_ActiveCollisions.erase(it);
            } else {
                ++it;
            }
        }

        // Remove from mapping
        s_CollisionObjectToEntity.erase(collisionObject);
    }

    void CollisionSystem::checkCollisions(const PhysicsWorld& world) {

        std::unordered_set<std::pair<btCollisionObject*, btCollisionObject*>, PairHash> currentCollisions;

        int numManifolds = world.getDynamicsWorld()->getDispatcher()->getNumManifolds();
        for (int i = 0; i < numManifolds; i++) {
            ::btPersistentManifold* contactManifold = world.getDynamicsWorld()->getDispatcher()->getManifoldByIndexInternal(i);
            auto objA = const_cast<btCollisionObject*>(contactManifold->getBody0());
            auto objB = const_cast<btCollisionObject*>(contactManifold->getBody1());

            // Get entities from collision objects
            Entity entityA(static_cast<entt::entity>(reinterpret_cast<size_t>(objA->getUserPointer())), s_Scene);
            Entity entityB(static_cast<entt::entity>(reinterpret_cast<size_t>(objB->getUserPointer())), s_Scene);

            if (contactManifold->getNumContacts() > 0) {
                auto pair = std::make_pair(objA, objB);
                currentCollisions.insert(pair);

                // CollisionInfo info = { entityA, entityB, contactManifold };
                CollisionInfo info{entityA, entityB, contactManifold};

                // Handle collision enter
                if (s_ActiveCollisions.find(pair) == s_ActiveCollisions.end()) {
                    if (entityA.HasComponent<RigidbodyComponent>() && entityB.HasComponent<RigidbodyComponent>()) {
                        auto& rbA = entityA.GetComponent<RigidbodyComponent>();
                        auto& rbB = entityB.GetComponent<RigidbodyComponent>();

                        if (rbA.callback.GetOnCollisionEnter())
                            rbA.callback.GetOnCollisionEnter()(info);
                        if (rbB.callback.GetOnCollisionEnter())
                            rbB.callback.GetOnCollisionEnter()(info);
                    }
                }
                // Handle collision stay
                else {
                    if (entityA.HasComponent<RigidbodyComponent>() && entityB.HasComponent<RigidbodyComponent>())
                    {
                        auto& rbA = entityA.GetComponent<RigidbodyComponent>();
                        auto& rbB = entityB.GetComponent<RigidbodyComponent>();

                        if (rbA.callback.GetOnCollisionStay())
                            rbA.callback.GetOnCollisionStay()(info);
                        if (rbB.callback.GetOnCollisionStay())
                            rbB.callback.GetOnCollisionStay()(info);
                    }
                }
            }
        }

        // Handle collision exit
        for (const auto& pair : s_ActiveCollisions) {
            if (currentCollisions.find(pair) == currentCollisions.end()) {
                Entity entityA(static_cast<entt::entity>(reinterpret_cast<size_t>(pair.first->getUserPointer())), s_Scene);
                Entity entityB(static_cast<entt::entity>(reinterpret_cast<size_t>(pair.second->getUserPointer())), s_Scene);

                CollisionInfo info = { entityA, entityB, nullptr };

                if (entityA.HasComponent<RigidbodyComponent>() && entityB.HasComponent<RigidbodyComponent>())
                {
                    auto& rbA = entityA.GetComponent<RigidbodyComponent>();
                    auto& rbB = entityB.GetComponent<RigidbodyComponent>();

                    if (rbA.callback.GetOnCollisionExit())
                        rbA.callback.GetOnCollisionExit()(info);
                    if (rbB.callback.GetOnCollisionExit())
                        rbB.callback.GetOnCollisionExit()(info);
                }
            }
        }

        s_ActiveCollisions = currentCollisions;
    }

    void CollisionSystem::Shutdown() {
        s_ActiveCollisions.clear();
        s_Scene = nullptr;
    }

}