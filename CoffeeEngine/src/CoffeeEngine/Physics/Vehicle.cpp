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

       // 根据按键设置转向输入
        if (moveLeft)
        {
            turnInput = 1.0f;
        }
        else if (moveRight)
        {
            turnInput = -1.0f;
        }
        else
        {
            turnInput = 0.0f; // 无输入时回正
        }

        // 计算转向速度
        float turnFactor = glm::clamp(speed / maxSpeed, 0.2f, 1.0f); // 速度对转向的影响（最低 10% 转向能力）
        currentTurnSpeed = maxTurnSpeed * turnFactor * turnInput;

        // 更新方向角（仅当车辆移动时）
        if (glm::abs(speed) > 0.1f)
        { // 添加一个速度阈值，避免静止时转向
            rotation += currentTurnSpeed * dt;
        }

        // 处理加速和减速
        if (moveFront)
        {
            speed += acceleration * dt;
            if (speed > maxSpeed)
            {
                speed = maxSpeed; // 限制最大速度
            }
        }
        else if (moveBack)
        {
            speed -= acceleration * dt;
            if (speed < -maxSpeed / 2.0f)
            { // 倒车速度限制为最大速度的一半
                speed = -maxSpeed / 2.0f;
            }
        }
        else
        {
            // 如果没有按键，应用阻力（速度慢慢减为 0）
            if (speed > 0)
            {
                speed -= drag * dt;
                if (speed < 0)
                    speed = 0;
            }
            else if (speed < 0)
            {
                speed += drag * dt;
                if (speed > 0)
                    speed = 0;
            }
        }

        // 计算速度向量
        float radians = glm::radians(rotation); // 将角度转为弧度
        glm::vec3 velocity = glm::vec3(std::sin(radians), 0.0f, std::cos(radians)) * speed;

        if (selectedEntity)
        {
            auto& transform = selectedEntity.GetComponent<TransformComponent>();
            // 更新实体的位置
            transform.Position += velocity * dt;
            if (glm::abs(speed) > 0.1f)
            { // 避免静止时改变旋转
                transform.Rotation.y = rotation;
            }
        }
    }

    void Vehicle::OnEvent(Coffee::Event& event)
    {

        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyPressedEvent>(COFFEE_BIND_EVENT_FN(Vehicle::OnKeyPressed));
        dispatcher.Dispatch<KeyReleasedEvent>(COFFEE_BIND_EVENT_FN(Vehicle::OnKeyRelease));
    }

    bool Vehicle::OnKeyPressed(KeyPressedEvent& event)
    {
        if (event.IsRepeat() > 0)
            return false;

        if (selectedEntity)
        {
            const float moveSpeed = 1.0f; // 移动速度
            auto& position = selectedEntity.GetComponent<TransformComponent>().Position;

            if (selectedEntity.GetComponent<TagComponent>().Tag == "FINAL_MODEL_74.fbx")
            {

                if (event.GetKeyCode() == Coffee::Key::W)
                {
                    moveFront = true;
                }

                switch (event.GetKeyCode())
                {
                case Coffee::Key::W:
                    moveFront = true;
                    break;
                case Coffee::Key::S:
                    moveBack = true;
                    break;
                case Coffee::Key::A:
                    moveLeft = true;
                    break;
                case Coffee::Key::D:
                    moveRight = true;
                    break;
                default:
                    return false;
                }
            }
        }
        return true;
    }

    bool Vehicle::OnKeyRelease(KeyReleasedEvent& event)
    {
        std::string key = event.ToString();
        printf("\n%s", key.c_str());

        if (selectedEntity)
        {
            const float moveSpeed = 1.0f; // 移动速度
            auto& position = selectedEntity.GetComponent<TransformComponent>().Position;

            if (selectedEntity.GetComponent<TagComponent>().Tag == "FINAL_MODEL_74.fbx")
            {

                if (key == "KeyReleasedEvent: 26")
                {
                    moveFront = false;
                }
                if (key == "KeyReleasedEvent: 22")
                {
                    moveBack = false;
                }
                if (key == "KeyReleasedEvent: 7")
                {
                    moveRight = false;
                }
                if (key == "KeyReleasedEvent: 4")
                {
                    moveLeft = false;
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