#include "Vehicle.h"
#include <CoffeeEngine/Scene/Components.h>

namespace Coffee
{

    Vehicle::Vehicle(double mass, double initialX, double initialY)
        : mass(mass), positionX(initialX), positionY(initialY), velocityX(0.0), velocityY(0.0), accelerationX(0.0),
          accelerationY(0.0), forceX(0.0), forceY(0.0)
    {
    }

    // 更新车辆状态
    void Vehicle::update(float dt)
    {
        // velocityX += accelerationX * deltaTime; // 更新速度
        // velocityY += accelerationY * deltaTime;

        // positionX += velocityX * deltaTime; // 更新位置
        // positionY += velocityY * deltaTime;

        selectedEntity = m_SceneTreePanel.GetSelectedEntity();
        if (selectedEntity)
        {
            // m_SceneTreePanel.GetSelectedEntity().GetComponent<TransformComponent>().Position.y = 100;
            COFFEE_CORE_WARN("Tag: {0}", selectedEntity.GetComponent<TransformComponent>().Position.x);
            selectedEntity.GetComponent<TransformComponent>().Position.x = 10;
        }
    }

    void Vehicle::OnEvent(Coffee::Event& event)
    {

        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyPressedEvent>(COFFEE_BIND_EVENT_FN(Vehicle::OnKeyPressed));
    }

    bool Vehicle::OnKeyPressed(KeyPressedEvent& event)
    {
        /*if (event.IsRepeat() > 0)
            return false;*/

        if (selectedEntity)
        {
             const float moveSpeed = 1.0f; // 移动速度
             auto& position = selectedEntity.GetComponent<TransformComponent>().Position;

            if (selectedEntity.GetComponent<TagComponent>().Tag == "FINAL_MODEL_74.fbx")
            {
                switch (event.GetKeyCode())
                {
                case Coffee::Key::W:
                    position.z += moveSpeed; // 前进
                    break;
                case Coffee::Key::S:
                     position.z -= moveSpeed; // 后退
                    break;
                case Coffee::Key::A:
                     position.x += moveSpeed; // 左移
                    break;
                case Coffee::Key::D:
                     position.x -= moveSpeed; // 右移
                    break;
                default:
                    return false;
                }
            }
        }
        return true;
    }

    // 获取 X 轴位置
    double Vehicle::getPositionX() const
    {
        return positionX;
    }

    // 获取 Y 轴位置
    double Vehicle::getPositionY() const
    {
        return positionY;
    }

    // 获取 X 轴速度
    double Vehicle::getVelocityX() const
    {
        return velocityX;
    }

    // 获取 Y 轴速度
    double Vehicle::getVelocityY() const
    {
        return velocityY;
    }

    void Vehicle::setEntity(Entity entity)
    {
        selectedEntity = entity;
    }

    Entity Vehicle::getEntity()
    {
        if (selectedEntity)
        {
            // printf("b");
        }

        return selectedEntity;
    }

} // namespace Coffee