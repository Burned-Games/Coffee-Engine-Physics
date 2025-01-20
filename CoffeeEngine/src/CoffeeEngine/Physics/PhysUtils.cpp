#include "PhysUtils.h"

namespace Coffee {
    
    btVector3 PhysUtils::GlmToBullet(const glm::vec3& v)
    {
        return btVector3(v.x, v.y, v.z);
    }

    glm::vec3 PhysUtils::BulletToGlm(const btVector3& v)
    {
        return glm::vec3(v.x(), v.y(), v.z());
    }
    
} // Coffee