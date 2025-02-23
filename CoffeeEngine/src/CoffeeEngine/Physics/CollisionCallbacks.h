/**
 * @file CollisionCallbacks.h
 * @brief Defines the CollisionCallbacks struct for handling collision events.
 */

#pragma once
#include <functional>

namespace Coffee
{
    class Collider;
    class RigidBody;

    /**
     * @struct CollisionCallbacks
     * @brief Contains callbacks for handling collision events.
     */
    struct CollisionCallbacks {
        using OnCollisionCallback = std::function<void(CollisionCallbacks* other)>;
    public:
        RigidBody* rigidBody; ///< Pointer to the associated RigidBody.
        Collider* collider; ///< Pointer to the associated Collider.
  
        OnCollisionCallback m_OnContactStarted; ///< Callback triggered when contact starts.
        OnCollisionCallback m_OnContactEnded; ///< Callback triggered when contact ends.
    };

} // Coffee
    

