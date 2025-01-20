//
// Created by Roger on 20/1/2025.
//

#include "DebugDrawer.h"

#include "CoffeeEngine/Renderer/DebugRenderer.h"
#include "PhysUtils.h"

namespace Coffee {

    using namespace Coffee;
    DebugDrawer::DebugDrawer() : mode(DBG_DrawWireframe)
    {
        
    }
    
    void DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
    {
        DebugRenderer::DrawLine(PhysUtils::BulletToGlm(from), PhysUtils::BulletToGlm(to), glm::vec4(PhysUtils::BulletToGlm(color),1));
    }
    
    void DebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
    {
        
    }
    
    void DebugDrawer::reportErrorWarning(const char* warningString)
    {
        COFFEE_WARN(warningString);
    }
    
    void DebugDrawer::draw3dText(const btVector3& location, const char* textString)
    {
        COFFEE_TRACE(("DebugDrawer::draw3dText((%i,%i,%i),%s)\n"), location.x(),location.y(),location.z(), textString);
    }
    
    void DebugDrawer::setDebugMode(int debugMode)
    {
        mode = (DebugDrawModes) debugMode;
    }
    
    int DebugDrawer::getDebugMode() const
    {
        return mode;
    }
    

} // Coffee