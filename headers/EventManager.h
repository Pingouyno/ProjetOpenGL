#ifndef EVENT_CLASS_H
#define EVENT_CLASS_H

#include"../libraries/include/GLAD/glad.h"
#include"../libraries/include/glm/gtc/matrix_transform.hpp"
#include"../libraries/include/glm/gtc/type_ptr.hpp"
#include"../libraries/include/glm/gtx/rotate_vector.hpp"
#include"../libraries/include/glm/gtx/vector_angle.hpp"

#include"PlaySound.h"
#include"World.h"

class EventManager
{
    private:
        World* world;
        Camera* camera;

        bool waitingForTPress = true;
        bool waitingForLClick = true;
        bool firstClick = true;

    public:
        EventManager(World* world);
        void Inputs(GLFWwindow* window);
    
    private:
        void checkKeyEvents(GLFWwindow* window);
        void checkMouseEvents(GLFWwindow* window); 
        
        //à activer et désactiver au besoin (raisons de performance)
        void checkKeyboardCamMovement(GLFWwindow* window);
};
#endif