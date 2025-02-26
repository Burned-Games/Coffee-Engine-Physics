#include "Collider.h"
#include "PhysUtils.h"
#include "PhysicsEngine.h"

namespace Coffee
{

    Collider::Collider(const CollisionShapeConfig& config, const glm::vec3& position, const glm::quat& rotation,
                       const glm::vec3& scale)
        : m_isTrigger(config.isTrigger), m_mass(config.mass), m_position(position), m_scale(scale)
    {
        // Usar la función CreateCollisionObject para crear el btCollisionObject
        m_collisionObject = PhysicsEngine::CreateCollisionObject(config, position, scale, rotation);

        // PhysicsEngine::AddDebugDrawCommand(CollisionShapeType::BOX, position, rotation,
        //                                    scale, // fullsize
        //                                    glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

        // Añadir el objeto al mundo físico (ya se hace en CreateCollisionObject)
    }

    Collider::~Collider()
    {
        PhysicsEngine::GetWorld()->removeCollisionObject(m_collisionObject);
        delete m_collisionObject;
    }

    void Collider::SetPosition(const glm::vec3& position, const glm::vec3& positionOffset)
    {
        if (this != nullptr && m_collisionObject)
        {
            // Actualizar la posición interna
            m_offset = positionOffset;
            m_position = position + m_offset;

            // Obtener la transformación actual del objeto de colisión
            btTransform transform = m_collisionObject->getWorldTransform();
            transform.setOrigin(PhysUtils::GlmToBullet(m_position));

            // Si el objeto es un btRigidBody, actualizar su estado de interpolación
            btRigidBody* rigidBody = btRigidBody::upcast(m_collisionObject);
            if (rigidBody)
            {
                rigidBody->setWorldTransform(transform);
                rigidBody->getMotionState()->setWorldTransform(transform);
            }
            else
            {
                m_collisionObject->setWorldTransform(transform);
            }

            // Redibujar la caja en la nueva posición para depuración
            Coffee::DebugRenderer::DrawBox(m_position, glm::vec3(0, 0, 0), m_scale, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),
                                           true, 2.0f);
        }
    }

    void Collider::ColliderUpdate(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& size,
                                  const glm::vec3& offset)
    {
        if (this != nullptr && m_collisionObject)
        {
            // Actualizar la posición interna
            m_offset = offset;
            m_position = position + m_offset;
            m_scale = size;
            

            // Obtener la transformación actual del objeto de colisión
            btTransform transform = m_collisionObject->getWorldTransform();
            transform.setOrigin(PhysUtils::GlmToBullet(m_position));

            // Si el objeto es un btRigidBody, actualizar su estado de interpolación
            btRigidBody* rigidBody = btRigidBody::upcast(m_collisionObject);
            if (rigidBody)
            {
                rigidBody->setWorldTransform(transform);
                rigidBody->getMotionState()->setWorldTransform(transform);
            }
            else
            {
                m_collisionObject->setWorldTransform(transform);
            }

            // Redibujar la caja en la nueva posición para depuración
            Coffee::DebugRenderer::DrawBox(m_position, rotation, m_scale, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),
                                           true, 2.0f);
        }
    }





    void Collider::SetSize(const glm::vec3& size, const glm::vec3& sizeOffset)
    {
        if (this != nullptr)
        {

            /*m_scale = size + sizeOffset;


            btTransform transform = m_collisionObject->getWorldTransform();


            btCollisionShape* shape = m_collisionObject->getCollisionShape();
            if (shape)
            {

                btBoxShape* boxShape = dynamic_cast<btBoxShape*>(shape);
                if (boxShape)
                {
                    boxShape->setImplicitShapeDimensions(
                        PhysUtils::GlmToBullet(m_scale * 0.5f));
                }
            }


            m_collisionObject->setWorldTransform(transform);*/
        }
    }

    glm::vec3 Collider::GetPosition() const
    {
        return m_position;
    }

    void Collider::SetEnabled(bool enabled)
    {
        if (enabled)
        {
            m_collisionObject->setCollisionFlags(m_collisionObject->getCollisionFlags() &
                                                 ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }
        else
        {
            /* m_collisionObject->setCollisionFlags(m_collisionObject->getCollisionFlags() |
                                                  btCollisionObject::CF_NO_CONTACT_RESPONSE);*/
        }
    }

    bool Collider::IsEnabled() const
    {
        return !(m_collisionObject->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE);
    }

    void Collider::AddCollisionListener(const CollisionCallback& callback)
    {
        m_collisionListeners.push_back(callback);
    }

    void Collider::OnCollision(Collider* other)
    {
        for (auto& listener : m_collisionListeners)
        {
            listener(other);
        }
    }

    // BOX COLLIDER

    // BoxCollider::BoxCollider(const glm::vec3& size, const glm::vec3& position, bool isTrigger, float mass)
    //     : Collider(isTrigger, mass), m_size(size)
    //{
    //     UpdateCollisionShape();
    //     SetPosition(position);
    // }

    // BoxCollider::~BoxCollider()
    //{
    //     // La l�gica de limpieza est?en la clase base
    // }

    // void BoxCollider::UpdateCollisionShape()
    //{
    //     btCollisionShape* shape = new btBoxShape(PhysUtils::GlmToBullet(m_size * 0.5f));
    //     m_collisionObject->setCollisionShape(shape);
    // }

    //// CAPSULE COLLIDER

    // CapsuleCollider::CapsuleCollider(float radius, float height, const glm::vec3& position, bool isTrigger, float
    // mass)
    //     : Collider(isTrigger, mass), m_radius(radius), m_height(height)
    //{
    //     UpdateCollisionShape();
    //     SetPosition(position);
    // }

    // CapsuleCollider::~CapsuleCollider()
    //{
    //     // La l�gica de limpieza est� en la clase base
    // }

    // void CapsuleCollider::SetRadius(float radius)
    //{
    //     if (m_radius != radius)
    //     {
    //         m_radius = radius;
    //         UpdateCollisionShape();
    //     }
    // }

    // float CapsuleCollider::GetRadius() const
    //{
    //     return m_radius;
    // }

    // void CapsuleCollider::SetHeight(float height)
    //{
    //     if (m_height != height)
    //     {
    //         m_height = height;
    //         UpdateCollisionShape();
    //     }
    // }

    // float CapsuleCollider::GetHeight() const
    //{
    //     return m_height;
    // }

    // void CapsuleCollider::UpdateCollisionShape()
    //{
    //     // Crear una nueva forma de colisi�n para la c�psula
    //     btCollisionShape* shape = new btCapsuleShape(m_radius, m_height);
    //     m_collisionObject->setCollisionShape(shape);
    // }

    //// CYLINDER COLLIDER

    // CylinderCollider::CylinderCollider(const glm::vec3& dimensions, const glm::vec3& position, bool isTrigger, float
    // mass)
    //     : Collider(isTrigger, mass), m_dimensions(dimensions)
    //{
    //     UpdateCollisionShape();
    //     SetPosition(position);
    // }

    // CylinderCollider::~CylinderCollider()
    //{
    //     // La l�gica de limpieza est� en la clase base
    // }

    // void CylinderCollider::SetDimensions(const glm::vec3& dimensions)
    //{
    //     if (m_dimensions != dimensions)
    //     {
    //         m_dimensions = dimensions;
    //         UpdateCollisionShape();
    //     }
    // }

    // glm::vec3 CylinderCollider::GetDimensions() const
    //{
    //     return m_dimensions;
    // }

    // void CylinderCollider::UpdateCollisionShape()
    //{
    //     // Crear una nueva forma de colisi�n para el cilindro
    //     btVector3 halfExtents = PhysUtils::GlmToBullet(m_dimensions * 0.5f); // Convertir dimensiones a half extents
    //     btCollisionShape* shape = new btCylinderShape(halfExtents);
    //     m_collisionObject->setCollisionShape(shape);
    // }

    //// PLANE COLLIDER

    // PlaneCollider::PlaneCollider(const glm::vec3& normal, float constant, const glm::vec3& position)
    //     : Collider(false, 0.0f), m_normal(normal), m_constant(constant)
    //{
    //     UpdateCollisionShape();
    //     SetPosition(position);
    // }

    // PlaneCollider::~PlaneCollider()
    //{
    //     // La l�gica de limpieza est� en la clase base
    // }

    // void PlaneCollider::SetNormal(const glm::vec3& normal)
    //{
    //     if (m_normal != normal)
    //     {
    //         m_normal = normal;
    //         UpdateCollisionShape();
    //     }
    // }

    // glm::vec3 PlaneCollider::GetNormal() const
    //{
    //     return m_normal;
    // }

    // void PlaneCollider::SetConstant(float constant)
    //{
    //     if (m_constant != constant)
    //     {
    //         m_constant = constant;
    //         UpdateCollisionShape();
    //     }
    // }

    // float PlaneCollider::GetConstant() const
    //{
    //     return m_constant;
    // }

    // void PlaneCollider::UpdateCollisionShape()
    //{
    //     // Crear una nueva forma de colisi�n para el plano
    //     btCollisionShape* shape = new btStaticPlaneShape(PhysUtils::GlmToBullet(m_normal), m_constant);
    //     m_collisionObject->setCollisionShape(shape);
    // }

    //// SPHERE COLLIDER

    // SphereCollider::SphereCollider(float radius, const glm::vec3& position, bool isTrigger, float mass)
    //     : Collider(isTrigger, mass), m_radius(radius)
    //{
    //     UpdateCollisionShape();
    //     SetPosition(position);
    // }

    // SphereCollider::~SphereCollider()
    //{
    //     // La l�gica de limpieza est?en la clase base
    // }

    // void SphereCollider::SetRadius(float radius)
    //{
    //     if (m_radius != radius)
    //     {
    //         m_radius = radius;
    //         UpdateCollisionShape();
    //     }
    // }

    // float SphereCollider::GetRadius() const
    //{
    //     return m_radius;
    // }

    // void SphereCollider::UpdateCollisionShape()
    //{
    //     btCollisionShape* shape = new btSphereShape(m_radius);
    //     m_collisionObject->setCollisionShape(shape);
    // }

} // namespace Coffee
