#pragma once
#include "CoffeeEngine/Physics/RigidBody.h"

#include <cereal/cereal.hpp>

namespace cereal {

    /**
     * @brief Serializes a glm::vec2 object.
     * @tparam Archive The type of the archive.
     * @param archive The archive to serialize to.
     * @param cfg The Coffee::CollisionShapeConfig object to serialize.
     */
    template<class Archive>
    void serialize(Archive& archive, Coffee::CollisionShapeConfig& cfg)
    {
        archive(CEREAL_NVP(cfg.type),
                CEREAL_NVP(cfg.size),
                CEREAL_NVP(cfg.isTrigger),
                CEREAL_NVP(cfg.mass)
                );
    }

    /**
     * @brief Serializes a glm::vec2 object.
     * @tparam Archive The type of the archive.
     * @param archive The archive to serialize to.
     * @param cfg The Coffee::RigidBodyConfig object to serialize.
     */
    template<class Archive>
    void serialize(Archive& archive, Coffee::RigidBodyConfig& cfg)
    {
        archive(CEREAL_NVP(cfg.shapeConfig),
                CEREAL_NVP(cfg.type),
                CEREAL_NVP(cfg.UseGravity),
                CEREAL_NVP(cfg.transform),
                CEREAL_NVP(cfg.Velocity),
                CEREAL_NVP(cfg.Acceleration),
                CEREAL_NVP(cfg.LinearDrag),
                CEREAL_NVP(cfg.AngularDrag),
                CEREAL_NVP(cfg.FreezeX),
                CEREAL_NVP(cfg.FreezeY),
                CEREAL_NVP(cfg.FreezeZ),
                CEREAL_NVP(cfg.FreezeRotX),
                CEREAL_NVP(cfg.FreezeRotY),
                CEREAL_NVP(cfg.FreezeRotZ)
                );
    }

} // cereal
