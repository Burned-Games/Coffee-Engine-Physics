#pragma once

#define BT_NO_SIMD_OPERATOR_OVERLOADS

#include "CoffeeEngine/Core/Log.h"
#include "CoffeeEngine/Physics/PhysicsWorld.h"
#include "CoffeeEngine/Scene/Entity.h"
#include <btBulletDynamicsCommon.h>
#include <unordered_set>
#include <utility>
#include <btBulletDynamicsCommon.h>

namespace Coffee {

    // Helper struct to hash collision pair
    struct PairHash {
        template <class T1, class T2>
        std::size_t operator()(const std::pair<T1, T2>& p) const {
            return std::hash<T1>{}(p.first) ^ std::hash<T2>{}(p.second);
        }
    };

    class CollisionSystem {
    public:
        static void Initialize(Scene* scene);
        static void RegisterCollider(Entity entity, btCollisionObject* collisionObject);
        static void RegisterRigidbody(Entity entity, btRigidBody* rigidBody);
        static void UnregisterCollisionObject(btCollisionObject* collisionObject);
        static void checkCollisions(const PhysicsWorld& world);
        static void Shutdown();

      private:
        static Scene* s_Scene;
        static std::unordered_map<btCollisionObject*, Entity> s_CollisionObjectToEntity;
        static std::unordered_set<std::pair<btCollisionObject*, btCollisionObject*>, PairHash> s_ActiveCollisions;
    };

}