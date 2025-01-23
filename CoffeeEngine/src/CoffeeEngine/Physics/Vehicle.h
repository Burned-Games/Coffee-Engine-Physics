#ifndef VEHICLE_H
#define VEHICLE_H
#include <SDL3/SDL.h> // 引入 SDL3 头文件以支持键盘输入
#include "../../../../CoffeeEditor/Panels/SceneTreePanel.h"
#include <CoffeeEngine/Events/KeyEvent.h>



namespace Coffee
{

    class Vehicle
    {
      public:
        // 默认构造函数
        Vehicle()
            : mass(0.0), positionX(0.0), positionY(0.0), velocityX(0.0), velocityY(0.0), accelerationX(0.0),
              accelerationY(0.0), forceX(0.0), forceY(0.0)
        {
            // 初始化成员变量为默认值
        }

        // 原有的构造函数
        Vehicle(double mass, double initialX = 0.0, double initialY = 0.0);

      private:
        double mass;          // 车辆质量 (kg)
        double positionX;     // X 轴位置 (m)
        double positionY;     // Y 轴位置 (m)
        double velocityX;     // X 轴速度 (m/s)
        double velocityY;     // Y 轴速度 (m/s)
        double accelerationX; // X 轴加速度 (m/s^2)
        double accelerationY; // Y 轴加速度 (m/s^2)

        double forceX; // X 轴上的当前外力 (N)
        double forceY; // Y 轴上的当前外力 (N)

        const double forceAmount = 500.0; // 每次按键施加的力大小 (N)

        
        Entity selectedEntity;

      public:

        // 更新方法
        void handleInput(const Uint8* keyState); // 处理键盘输入
        void update();           // 更新车辆状态


        void OnEvent(Coffee::Event& event);

        bool OnKeyPressed(KeyPressedEvent& event);

        SceneTreePanel m_SceneTreePanel;
        // 获取方法
        double getPositionX() const;
        double getPositionY() const;
        double getVelocityX() const;
        double getVelocityY() const;

        void setEntity(Entity entity);       
        Entity getEntity();       

        //ImGuiLayer* m_ImGuiLayer;
    };

    
} // namespace Coffee
#endif // VEHICLE_H
