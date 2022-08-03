#ifndef EVENT_CLASS_H
#define EVENT_CLASS_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include"../libraries/include/glm/gtc/matrix_transform.hpp"
#include"../libraries/include/glm/gtc/type_ptr.hpp"
#include"../libraries/include/glm/gtx/rotate_vector.hpp"
#include"../libraries/include/glm/gtx/vector_angle.hpp"

#include"World.h"

class EventManager
{
    private:
        World *world;
        Camera *camera;

    public:
        EventManager(World *world);
        void Inputs(GLFWwindow* window);
    
    private:
        void checkKeyEvents(GLFWwindow* window);
        void checkMouseEvents(GLFWwindow* window); 
};
#endif