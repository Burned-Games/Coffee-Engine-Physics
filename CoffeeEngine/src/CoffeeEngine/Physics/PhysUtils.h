#pragma once
#include <LinearMath/btTransform.h>
#include <LinearMath/btVector3.h>
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

namespace Coffee
{

    class PhysUtils
    {
    public:
        
        static btVector3 GlmToBullet(const glm::vec3& v);
        static glm::vec3 BulletToGlm(const btVector3& v);

        static glm::mat4 Mat4BulletToGlm(const btTransform& t);
        
        
    };

} // Coffee