#pragma once

#define BT_NO_SIMD_OPERATOR_OVERLOADS

#include <btBulletDynamicsCommon.h>
#include <cereal/access.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>
#include <glm/detail/type_quat.hpp>
#include <glm/glm.hpp>

namespace Coffee {

    class BoxCollider;
    class SphereCollider;
    class CapsuleCollider;

    class Collider {
    public:
        virtual ~Collider() {
            if (m_Shape)
            {
                delete m_Shape;
                m_Shape = nullptr;
            }

            if (m_CollisionObject) {
                delete m_CollisionObject;
                m_CollisionObject = nullptr;
            }
        }

        btCollisionShape* getShape() const { return m_Shape; }

        btCollisionObject* getCollisionObject() const { return m_CollisionObject; }

        // Initialize standalone collider (not tied to rigidbody)
        void InitializeStandalone() {
            if (!m_CollisionObject) {
                m_CollisionObject = new btCollisionObject();
                m_CollisionObject->setCollisionShape(m_Shape);
                m_CollisionObject->setCollisionFlags(m_CollisionObject->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
            }
        }

        // Set transform for standalone collider
        void SetTransform(const glm::vec3& position, const glm::quat& rotation) {
            if (m_CollisionObject) {
                btTransform transform;
                transform.setIdentity();
                transform.setOrigin(btVector3(position.x, position.y, position.z));
                transform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
                m_CollisionObject->setWorldTransform(transform);
            }
        }

        // Set trigger status
        void SetTrigger(bool isTrigger) {
            m_IsTrigger = isTrigger;
            if (m_CollisionObject) {
                if (isTrigger) {
                    m_CollisionObject->setCollisionFlags(m_CollisionObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
                } else {
                    m_CollisionObject->setCollisionFlags(m_CollisionObject->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
                }
            }
        }

        bool IsTrigger() const { return m_IsTrigger; }

    protected:
        btCollisionShape* m_Shape = nullptr;
        btCollisionObject* m_CollisionObject = nullptr;
        bool m_IsTrigger = false;
        
        // Add friend declaration for cereal
        friend class cereal::access;
        
        // Add template methods for serialization
        template<class Archive>
        void save(Archive& archive) const {
            btCollisionShape* shape = getShape();
            int shapeType = shape ? shape->getShapeType() : -1;
            archive(cereal::make_nvp("ShapeType", shapeType));
            archive(cereal::make_nvp("IsTrigger", m_IsTrigger));
            if (shape) {
                switch (shapeType) {
                    case BOX_SHAPE_PROXYTYPE: {
                        auto* boxShape = static_cast<btBoxShape*>(shape);
                        btVector3 halfExtents = boxShape->getHalfExtentsWithoutMargin();
                        glm::vec3 size(halfExtents.x() * 2.0f, halfExtents.y() * 2.0f, halfExtents.z() * 2.0f);
                        archive(cereal::make_nvp("Size", size));
                        break;
                    }
                    case SPHERE_SHAPE_PROXYTYPE: {
                        auto* sphereShape = static_cast<btSphereShape*>(shape);
                        float radius = sphereShape->getRadius();
                        archive(cereal::make_nvp("Radius", radius));
                        break;
                    }
                    case CAPSULE_SHAPE_PROXYTYPE: {
                        auto* capsuleShape = static_cast<btCapsuleShape*>(shape);
                        float radius = capsuleShape->getRadius();
                        float height = capsuleShape->getHalfHeight() * 2.0f;
                        archive(cereal::make_nvp("Radius", radius));
                        archive(cereal::make_nvp("Height", height));
                        break;
                    }
                }
            }
        }

        template<class Archive>
        void load(Archive& archive) {
            int shapeType;
            archive(cereal::make_nvp("ShapeType", shapeType));
            try {
                archive(cereal::make_nvp("IsTrigger", m_IsTrigger));
            } catch (...) {
                m_IsTrigger = false;
            }

            if (m_Shape) {
                delete m_Shape;
                m_Shape = nullptr;
            }

            switch (shapeType) {
                case BOX_SHAPE_PROXYTYPE: {
                    glm::vec3 size;
                    archive(cereal::make_nvp("Size", size));
                    m_Shape = new btBoxShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f));
                    break;
                }
                case SPHERE_SHAPE_PROXYTYPE: {
                    float radius;
                    archive(cereal::make_nvp("Radius", radius));
                    m_Shape = new btSphereShape(radius);
                    break;
                }
                case CAPSULE_SHAPE_PROXYTYPE: {
                    float radius, height;
                    archive(cereal::make_nvp("Radius", radius));
                    archive(cereal::make_nvp("Height", height));
                    m_Shape = new btCapsuleShape(radius, height * 0.5f);
                    break;
                }
            }
        }
    };

    class BoxCollider : public Collider {
    public:
        BoxCollider(const glm::vec3& size = glm::vec3(1.0f, 1.0f, 1.0f)) {
            m_Size = size;
            m_Shape = new btBoxShape(btVector3(size.x / 2.0f, size.y / 2.0f, size.z / 2.0f));
        }

        const glm::vec3& GetSize() const { return m_Size; }

    private:
        glm::vec3 m_Size;

        friend class cereal::access;

        template<class Archive>
        void save(Archive& archive) const {
            archive(cereal::base_class<Collider>(this));
        }

        template<class Archive>
        void load(Archive& archive) {
            archive(cereal::base_class<Collider>(this));
        }
    };

    class SphereCollider : public Collider {
    public:
        SphereCollider(float radius = 0.5f) {
            m_Radius = radius;
            m_Shape = new btSphereShape(radius);
        }

        float GetRadius() const { return m_Radius; }

    private:
        float m_Radius;

        friend class cereal::access;

        template<class Archive>
        void save(Archive& archive) const {
            archive(cereal::base_class<Collider>(this));
        }

        template<class Archive>
        void load(Archive& archive) {
            archive(cereal::base_class<Collider>(this));
        }
    };

    class CapsuleCollider : public Collider {
    public:
        CapsuleCollider(float radius = 0.5f, float height = 1.0f) {
            m_Radius = radius;
            m_Height = height;
            m_Shape = new btCapsuleShape(radius, height);
        }

        float GetRadius() const { return m_Radius; }
        float GetHeight() const { return m_Height; }

    private:
        float m_Radius;
        float m_Height;

        friend class cereal::access;

        template<class Archive>
        void save(Archive& archive) const {
            archive(cereal::base_class<Collider>(this));
        }

        template<class Archive>
        void load(Archive& archive) {
            archive(cereal::base_class<Collider>(this));
        }
    };

} // namespace Coffee

CEREAL_REGISTER_TYPE(Coffee::BoxCollider)
CEREAL_REGISTER_TYPE(Coffee::SphereCollider)
CEREAL_REGISTER_TYPE(Coffee::CapsuleCollider)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Coffee::Collider, Coffee::BoxCollider)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Coffee::Collider, Coffee::SphereCollider)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Coffee::Collider, Coffee::CapsuleCollider)