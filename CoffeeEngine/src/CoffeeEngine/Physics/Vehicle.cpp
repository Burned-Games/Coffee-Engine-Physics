#include "Vehicle.h"
#include <CoffeeEngine/Scene/Components.h>

namespace Coffee
{

    Vehicle::Vehicle(double mass, double initialX, double initialY)
        : mass(mass), positionX(initialX), positionY(initialY), velocityX(0.0), velocityY(0.0), accelerationX(0.0),
          accelerationY(0.0), forceX(0.0), forceY(0.0)
    {
    }


    void Vehicle::update(float dt)
    {
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
            turnInput = 0.0f; // return if no input
        }

        // calc turn speed
        float turnFactor = glm::clamp(speed / maxSpeed, 0.2f, 1.0f); 
        currentTurnSpeed = maxTurnSpeed * turnFactor * turnInput;

       
        if (glm::abs(speed) > 0.1f)
        { 
            rotation += currentTurnSpeed * dt;
        }

   
        if (moveFront)
        {
            speed += acceleration * dt;
            if (speed > maxSpeed)
            {
                speed = maxSpeed;
            }
        }
        else if (moveBack)
        {
            speed -= acceleration * dt;
            if (speed < -maxSpeed / 2.0f)
            { 
                speed = -maxSpeed / 2.0f;
            }
        }
        else
        {
          
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

       
        float radians = glm::radians(rotation);
        glm::vec3 velocity = glm::vec3(std::sin(radians), 0.0f, std::cos(radians)) * speed;

        if (selectedEntity)
        {
            auto& transform = selectedEntity.GetComponent<TransformComponent>();
          
            transform.Position += velocity * dt;
            if (glm::abs(speed) > 0.1f)
            {
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

        if (selectedEntity)
        {

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