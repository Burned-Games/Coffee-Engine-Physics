#pragma once

#define BT_NO_SIMD_OPERATOR_OVERLOADS

#include "CoffeeEngine/Core/Base.h"
#include "PhysicsWorld.h"
#include "CoffeeEngine/Scene/Entity.h"

#include <glm/glm.hpp>
#include <vector>

namespace Coffee {

    struct RaycastHit {
        Entity hitEntity;             // Entity that was hit (if available)
        glm::vec3 position;           // Hit position in world
        glm::vec3 normal;             // Surface normal at hit point
        float distance;
        bool hasHit;
    };

    class RaycastSystem {
    public:

        // Cast a ray and return the first hit
        static RaycastHit Raycast(const PhysicsWorld& world, const glm::vec3& origin, const glm::vec3& direction, float maxDistance = 1000.0f);
        
        // Cast a ray and return all hits
        static std::vector<RaycastHit> RaycastAll(const PhysicsWorld& world, const glm::vec3& origin, const glm::vec3& direction, float maxDistance = 1000.0f);
    };

} // namespace Coffee
