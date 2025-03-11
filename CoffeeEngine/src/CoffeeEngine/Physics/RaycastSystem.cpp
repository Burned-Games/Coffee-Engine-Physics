#include "RaycastSystem.h"
#include "CollisionSystem.h"

#include <btBulletDynamicsCommon.h>

namespace Coffee {

    namespace {
        // Helper function to convert a raycast result to  RaycastHit structure
        RaycastHit ConvertRaycastResult(const btCollisionWorld::ClosestRayResultCallback& result) {
            RaycastHit hit;
            
            hit.hasHit = result.hasHit();
            
            if (hit.hasHit) {
                // hit position
                const btVector3& hitPoint = result.m_hitPointWorld;
                hit.position = glm::vec3(hitPoint.x(), hitPoint.y(), hitPoint.z());
                
                //  hit normal
                const btVector3& hitNormal = result.m_hitNormalWorld;
                hit.normal = glm::vec3(hitNormal.x(), hitNormal.y(), hitNormal.z());
                
                // Calculate distance
                hit.distance = result.m_closestHitFraction * result.m_rayToWorld.length();
                
                //  hit entity
                const btCollisionObject* hitObject = result.m_collisionObject;
                if (hitObject) {
                    Entity entity(static_cast<entt::entity>(reinterpret_cast<size_t>(hitObject->getUserPointer())), nullptr);
                    hit.hitEntity = entity;
                }
            }
            
            return hit;
        }
    }

    RaycastHit RaycastSystem::Raycast(const PhysicsWorld& world, const glm::vec3& origin, const glm::vec3& direction, float maxDistance) {
        //  start and end points
        btVector3 from(origin.x, origin.y, origin.z);
        btVector3 to = from + btVector3(direction.x, direction.y, direction.z) * maxDistance;
        
        //  raycast callback
        btCollisionWorld::ClosestRayResultCallback callback(from, to);
        
        //  raycast
        world.getDynamicsWorld()->rayTest(from, to, callback);
        
        return ConvertRaycastResult(callback);
    }

    std::vector<RaycastHit> RaycastSystem::RaycastAll(const PhysicsWorld& world, const glm::vec3& origin, const glm::vec3& direction, float maxDistance) {
        std::vector<RaycastHit> results;
        
        //  start and end points
        btVector3 from(origin.x, origin.y, origin.z);
        btVector3 to = from + btVector3(direction.x, direction.y, direction.z) * maxDistance;
        
        //  raycast callback for all results
        btCollisionWorld::AllHitsRayResultCallback callback(from, to);
        
        //  raycast
        world.getDynamicsWorld()->rayTest(from, to, callback);
        
        if (callback.hasHit()) {
            for (int i = 0; i < callback.m_hitPointWorld.size(); i++) {
                RaycastHit hit;
                hit.hasHit = true;
                
                //  hit position
                const btVector3& hitPoint = callback.m_hitPointWorld[i];
                hit.position = glm::vec3(hitPoint.x(), hitPoint.y(), hitPoint.z());
                
                //  hit normal
                const btVector3& hitNormal = callback.m_hitNormalWorld[i];
                hit.normal = glm::vec3(hitNormal.x(), hitNormal.y(), hitNormal.z());
                
                // Calculate distance
                hit.distance = callback.m_hitFractions[i] * (to - from).length();
                
                //  hit entity
                const btCollisionObject* hitObject = callback.m_collisionObjects[i];
                if (hitObject) {
                    Entity entity(static_cast<entt::entity>(reinterpret_cast<size_t>(hitObject->getUserPointer())), nullptr);
                    hit.hitEntity = entity;
                }
                
                results.push_back(hit);
            }
        }
        
        return results;
    }

} // namespace Coffee
