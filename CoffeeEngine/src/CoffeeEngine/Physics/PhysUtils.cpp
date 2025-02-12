#include "PhysUtils.h"

#include <LinearMath/btTransform.h>
#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Coffee {
    
    btVector3 PhysUtils::GlmToBullet(const glm::vec3& v)
    {
        return btVector3(v.x, v.y, v.z);
    }

    glm::vec3 PhysUtils::BulletToGlm(const btVector3& v)
    {
        return glm::vec3(v.x(), v.y(), v.z());
    }
    glm::mat4 PhysUtils::Mat4BulletToGlm(const btTransform& t)
    {
        glm::mat4 m(1.0f);
        t.getOpenGLMatrix(value_ptr(m));
        return m;
    }

} // Coffee