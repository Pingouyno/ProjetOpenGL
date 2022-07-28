#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include"../libraries/include/glm/glm.hpp"
#include"../libraries/include/glm/gtc/matrix_transform.hpp"
#include"../libraries/include/glm/gtc/type_ptr.hpp"
#include"../libraries/include/glm/gtx/rotate_vector.hpp"
#include"../libraries/include/glm/gtx/vector_angle.hpp"
#include"../libraries/include/IrrKlang/include/irrKlang.h"
using namespace irrklang;

#include"shaderClass.h"

class Camera
{
    public:
        // Stores the main vectors of the camera
        glm::vec3 Position;
        glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);

        glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);


        // Prevents the camera from jumping around when first clicking left click
        bool firstClick = true;
        bool isInAir = true;
        bool isInCreative = false;

        // Stores the width and height of the window
        int width;
        int height;

        // Adjust the speed of the camera and it's sensitivity when looking around
        float speed = NORMAL_SPEED;
        float sensitivity = 100.0f;
        //on ne met pas à 0 car cela ferait de mini-bonds au sol
        float DEFAULT_TIME_AIR = 5.0f;
        
        float timeInAir = DEFAULT_TIME_AIR;
        //longueur du saut (en frames)
        const float JUMP_LENGTH = 45.0f;
        //force G qui nous donne x * 1u/s d'accélération
        const float JUMP_FALL_ACCELERATION = 9.8 * 1.0f / 1830.0f; 

        // Camera constructor to set up initial values
        Camera(int width, int height, glm::vec3 position);

        // Updates and exports the camera matrix to the Vertex Shader
        void Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform);
        // Handles camera inputs
        void Inputs(GLFWwindow* window, ISoundEngine* soundEngine);
        
    private :
        const float NORMAL_SPEED = 0.10f;
        const float FAST_SPEED = 2 * NORMAL_SPEED;
        const float VERT_PAD_SENSITIVITY = 2.0f;
        const float HORI_PAD_SENSITIVITY = 2.8f;
        void checkCamMovement(GLFWwindow* window, ISoundEngine* soundEngine);
        void checkMouseMovement(GLFWwindow* window);
        void jump();
        void land();
        void fall();

};
#endif