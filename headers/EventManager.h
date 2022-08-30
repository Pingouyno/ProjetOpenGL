#ifndef EVENT_CLASS_H
#define EVENT_CLASS_H

#include"../libraries/include/GLAD/glad.h"
#include"../libraries/include/glm/gtc/matrix_transform.hpp"
#include"../libraries/include/glm/gtc/type_ptr.hpp"
#include"../libraries/include/glm/gtx/rotate_vector.hpp"
#include"../libraries/include/glm/gtx/vector_angle.hpp"

#include"PlaySound.h"
#include"World.h"
#include"MousePicker.h"

class EventManager
{
    private:
	    MousePicker* mousePicker;
        World* world;
        Camera* camera;

        bool waitingForTPress = true;
        bool waitingForLClick = true;
        bool waitingForRClick = true;
        bool waitingFor1Press = true;
        bool waitingFor2Press = true;

    public:
        EventManager(World* world);
        void Inputs(GLFWwindow* window);
    
    private:
        void checkKeyboardCamMovement(GLFWwindow* window);
        void checkMouseCamMovement(GLFWwindow* window); 

        void checkKeyboardEvents(GLFWwindow* window);
        void checkMouseEvents(GLFWwindow* window);

        void checkMoveAndPhysics(GLFWwindow* window);
};
#endif