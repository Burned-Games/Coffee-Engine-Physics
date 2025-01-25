#pragma once
#include <functional>

namespace Coffee
{
    class Collider;
    class RigidBody;


    struct CollisionCallbacks {
        using OnCollisionCallback = std::function<void(CollisionCallbacks* other)>;
    public:
        RigidBody* rigidBody;
        Collider* collider;
  
        OnCollisionCallback m_OnContactStarted;
        
        OnCollisionCallback m_OnContactEnded;
    };

} // Coffee
    

