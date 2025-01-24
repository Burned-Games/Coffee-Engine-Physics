#ifndef VEHICLE_H
#define VEHICLE_H
#include <SDL3/SDL.h> 
#include "../../../../CoffeeEditor/Panels/SceneTreePanel.h"
#include <CoffeeEngine/Events/KeyEvent.h>



namespace Coffee
{

    class Vehicle
    {
      public:
      
        Vehicle()
            : mass(0.0), positionX(0.0), positionY(0.0), velocityX(0.0), velocityY(0.0), accelerationX(0.0),
              accelerationY(0.0), forceX(0.0), forceY(0.0)
        {
            
        }

      
        Vehicle(double mass, double initialX = 0.0, double initialY = 0.0);

      private:
        double mass;         
        double positionX;    
        double positionY;    
        double velocityX;    
        double velocityY;     
        double accelerationX; 
        double accelerationY;

        double forceX; 
        double forceY; 

        const double forceAmount = 500.0;

        bool moveFront = false;
        bool moveBack = false;
        bool moveLeft = false;
        bool moveRight = false;

   
        float speed = 0.0f;         
        float maxSpeed = 50.0f;    
        float acceleration = 10.0f;
        float deceleration = 5.0f;  
        float maxTurnSpeed = 45.0f;
        float drag = 2.0f;         
        float rotation = 0.0f;      

      
        float turnInput = 0.0f;      
        float currentTurnSpeed = 0.0f; 

        Entity selectedEntity;
        SceneTreePanel m_SceneTreePanel;

      public:

        void update(float dt);         


        void OnEvent(Coffee::Event& event);

        bool OnKeyPressed(KeyPressedEvent& event);

        bool OnKeyRelease(KeyReleasedEvent& event);

        void setEntity(Entity entity);       
        Entity getEntity();       

        //ImGuiLayer* m_ImGuiLayer;
    };

    
} // namespace Coffee
#endif // VEHICLE_H
