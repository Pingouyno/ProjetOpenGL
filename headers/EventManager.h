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
        Player* player;

        bool waitingForTPress = true;
        bool waitingForLClick = true;
        bool waitingForRClick = true;
        bool waitingForKeyPress1Through9[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; //va de 1 à 9


    public:
        EventManager(World* world);
        void doEntityPhysics();
        void Inputs(GLFWwindow* window);
    
    private:
        void checkKeyboardCamMovement(GLFWwindow* window);
        void checkMouseCamMovement(GLFWwindow* window); 

        void checkKeyboardEvents(GLFWwindow* window);
        void checkMouseEvents(GLFWwindow* window);

        void checkMoveAndPhysics(GLFWwindow* window);
};
#endif