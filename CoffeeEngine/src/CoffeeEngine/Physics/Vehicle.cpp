#include "Vehicle.h"
#include <CoffeeEngine/Scene/Components.h>



namespace Coffee
{

    Vehicle::Vehicle(double mass, double initialX, double initialY)
        : mass(mass), positionX(initialX), positionY(initialY), velocityX(0.0), velocityY(0.0), accelerationX(0.0),
          accelerationY(0.0), forceX(0.0), forceY(0.0)
    {
    }

    // 处理键盘输入
    void Vehicle::handleInput(const Uint8* keyState)
    {
        forceX = 0.0; // 重置力
        forceY = 0.0;

        // 根据按键施加力
        if (keyState[SDL_SCANCODE_W])
        {                          // 按下 W 键
            forceY -= forceAmount; // 向上施加力
            printf("W");

            selectedEntity.GetComponent<TransformComponent>().Position.x += 1;
        }
        if (keyState[SDL_SCANCODE_S])
        {                          // 按下 S 键
            forceY += forceAmount; // 向下施加力
            printf("S");
        }
        if (keyState[SDL_SCANCODE_A])
        {                          // 按下 A 键
            forceX -= forceAmount; // 向左施加力
            printf("A");
        }
        if (keyState[SDL_SCANCODE_D])
        {                          // 按下 D 键
            forceX += forceAmount; // 向右施加力
            printf("D");
        }

        // 计算加速度
        if (mass > 0)
        {
            accelerationX = forceX / mass;
            accelerationY = forceY / mass;
        }
        else
        {
            accelerationX = 0.0;
            accelerationY = 0.0;
        }
    }

    // 更新车辆状态
    void Vehicle::update()
    {
        // velocityX += accelerationX * deltaTime; // 更新速度
        // velocityY += accelerationY * deltaTime;

        // positionX += velocityX * deltaTime; // 更新位置
        // positionY += velocityY * deltaTime;

         //printf("a");

        if (selectedEntity)
        {
            printf("yeess");
        }


        /* SDL_Event event;
        while (SDL_PollEvent(&event))
        {

            if (event.key.scancode, 0){
            
            }

            m_ImGuiLayer->ProcessEvents(event);
            switch (event.type)
            {
                if (event.key.repeat)
                {
                    KeyPressedEvent e(event.key.scancode, 1);

                }
                else
                {
                    KeyPressedEvent e(event.key.scancode, 0);

                }
            }
        }*/

    }

    void Vehicle::OnEvent(Coffee::Event& event)
    {

        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyPressedEvent>(COFFEE_BIND_EVENT_FN(Vehicle::OnKeyPressed));

    }

    bool Vehicle::OnKeyPressed(KeyPressedEvent& event)
    {
        //if (event.IsRepeat() > 0)
        //    return false;

        switch (event.GetKeyCode())
        {
        case Coffee::Key::Q:

               printf("sss");
            break;
        }
        return false;
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
        update();
        printf("s");
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