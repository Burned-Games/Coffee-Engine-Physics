#include "Collider.h"
#include "PhysUtils.h"
#include "PhysicsEngine.h"

namespace Coffee
{

    Collider::Collider(bool isStatic, bool isTrigger, float mass)
        : m_isStatic(isStatic), m_isTrigger(isTrigger), m_mass(mass), m_position(0.0f)
    {
        m_collisionObject = new btCollisionObject();
        if (m_isTrigger)
        {
            m_collisionObject->setCollisionFlags(m_collisionObject->getCollisionFlags() |
                                                 btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }
        PhysicsEngine::GetWorld()->addCollisionObject(m_collisionObject);
    }

    Collider::~Collider()
    {
        PhysicsEngine::GetWorld()->removeCollisionObject(m_collisionObject);
        delete m_collisionObject;
    }

    void Collider::SetPosition(const glm::vec3& position)
    {
        m_position = position;
        btTransform transform = m_collisionObject->getWorldTransform();
        transform.setOrigin(PhysUtils::GlmToBullet(m_position));
        m_collisionObject->setWorldTransform(transform);
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
            m_collisionObject->setCollisionFlags(m_collisionObject->getCollisionFlags() |
                                                 btCollisionObject::CF_NO_CONTACT_RESPONSE);
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

    BoxCollider::BoxCollider(const glm::vec3& size, const glm::vec3& position, bool isStatic, bool isTrigger,
                             float mass)
        : Collider(isStatic, isTrigger, mass), m_size(size)
    {
        UpdateCollisionShape();
        SetPosition(position);
    }

    BoxCollider::~BoxCollider()
    {
        // La lógica de limpieza est?en la clase base
    }

    void BoxCollider::UpdateCollisionShape()
    {
        btCollisionShape* shape = new btBoxShape(PhysUtils::GlmToBullet(m_size * 0.5f));
        m_collisionObject->setCollisionShape(shape);
    }

    // CAPSULE COLLIDER

    CapsuleCollider::CapsuleCollider(float radius, float height, const glm::vec3& position, bool isStatic,
                                     bool isTrigger, float mass)
        : Collider(isStatic, isTrigger, mass), m_radius(radius), m_height(height)
    {
        UpdateCollisionShape();
        SetPosition(position);
    }

    CapsuleCollider::~CapsuleCollider()
    {
        // La lógica de limpieza está en la clase base
    }

    void CapsuleCollider::SetRadius(float radius)
    {
        if (m_radius != radius)
        {
            m_radius = radius;
            UpdateCollisionShape();
        }
    }

    float CapsuleCollider::GetRadius() const
    {
        return m_radius;
    }

    void CapsuleCollider::SetHeight(float height)
    {
        if (m_height != height)
        {
            m_height = height;
            UpdateCollisionShape();
        }
    }

    float CapsuleCollider::GetHeight() const
    {
        return m_height;
    }

    void CapsuleCollider::UpdateCollisionShape()
    {
        // Crear una nueva forma de colisión para la cápsula
        btCollisionShape* shape = new btCapsuleShape(m_radius, m_height);
        m_collisionObject->setCollisionShape(shape);
    }

    // CYLINDER COLLIDER

    CylinderCollider::CylinderCollider(const glm::vec3& dimensions, const glm::vec3& position, bool isStatic,
                                       bool isTrigger, float mass)
        : Collider(isStatic, isTrigger, mass), m_dimensions(dimensions)
    {
        UpdateCollisionShape();
        SetPosition(position);
    }

    CylinderCollider::~CylinderCollider()
    {
        // La lógica de limpieza está en la clase base
    }

    void CylinderCollider::SetDimensions(const glm::vec3& dimensions)
    {
        if (m_dimensions != dimensions)
        {
            m_dimensions = dimensions;
            UpdateCollisionShape();
        }
    }

    glm::vec3 CylinderCollider::GetDimensions() const
    {
        return m_dimensions;
    }

    void CylinderCollider::UpdateCollisionShape()
    {
        // Crear una nueva forma de colisión para el cilindro
        btVector3 halfExtents = PhysUtils::GlmToBullet(m_dimensions * 0.5f); // Convertir dimensiones a half extents
        btCollisionShape* shape = new btCylinderShape(halfExtents);
        m_collisionObject->setCollisionShape(shape);
    }

    // PLANE COLLIDER

    PlaneCollider::PlaneCollider(const glm::vec3& normal, float constant, const glm::vec3& position)
        : Collider(true, false, 0.0f), m_normal(normal), m_constant(constant)
    {
        UpdateCollisionShape();
        SetPosition(position);
    }

    PlaneCollider::~PlaneCollider()
    {
        // La lógica de limpieza está en la clase base
    }

    void PlaneCollider::SetNormal(const glm::vec3& normal)
    {
        if (m_normal != normal)
        {
            m_normal = normal;
            UpdateCollisionShape();
        }
    }

    glm::vec3 PlaneCollider::GetNormal() const
    {
        return m_normal;
    }

    void PlaneCollider::SetConstant(float constant)
    {
        if (m_constant != constant)
        {
            m_constant = constant;
            UpdateCollisionShape();
        }
    }

    float PlaneCollider::GetConstant() const
    {
        return m_constant;
    }

    void PlaneCollider::UpdateCollisionShape()
    {
        // Crear una nueva forma de colisión para el plano
        btCollisionShape* shape = new btStaticPlaneShape(PhysUtils::GlmToBullet(m_normal), m_constant);
        m_collisionObject->setCollisionShape(shape);
    }

    // SPHERE COLLIDER

    SphereCollider::SphereCollider(float radius, const glm::vec3& position, bool isStatic, bool isTrigger, float mass)
        : Collider(isStatic, isTrigger, mass), m_radius(radius)
    {
        UpdateCollisionShape();
        SetPosition(position);
    }

    SphereCollider::~SphereCollider()
    {
        // La lógica de limpieza est?en la clase base
    }

    void SphereCollider::SetRadius(float radius)
    {
        if (m_radius != radius)
        {
            m_radius = radius;
            UpdateCollisionShape();
        }
    }

    float SphereCollider::GetRadius() const
    {
        return m_radius;
    }

    void SphereCollider::UpdateCollisionShape()
    {
        btCollisionShape* shape = new btSphereShape(m_radius);
        m_collisionObject->setCollisionShape(shape);
    }

} // namespace Coffee
