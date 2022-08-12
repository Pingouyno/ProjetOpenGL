#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include<vector>
#include<fstream>
using namespace std;

#include"shaderClass.h"
#include"../libraries/include/GLAD/glad.h"
#include"../libraries/include/glfw/glfw3.h"
#include"../libraries/include/glm/gtc/matrix_transform.hpp"
#include"../libraries/include/glm/gtc/type_ptr.hpp"
#include"../libraries/include/glm/gtx/rotate_vector.hpp"
#include"../libraries/include/glm/gtx/vector_angle.hpp"

class Camera
{
    public:
        static float NORMAL_SPEED;
        static float FAST_SPEED;
        static float VERT_PAD_SENSITIVITY;
        static float HORI_PAD_SENSITIVITY;
        //longueur du saut (en frames)
        static float JUMP_LENGTH;
        //force G qui nous donne x * 1u/s d'accélération
        static float JUMP_FALL_ACCELERATION;
    
        // Stores the main vectors of the camera
        glm::vec3 Position;
        glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

        // Prevents the camera from jumping around when first clicking left click
        bool isInAir = true;

        float width;
        float height;

        // Adjust the speed of the camera and it's sensitivity when looking around
        float speed = NORMAL_SPEED;
        float sensitivity = 100.0f;
        //on ne met pas à 0 car cela ferait de mini-bonds au sol
        float DEFAULT_TIME_AIR = 5.0f;
        float timeInAir = DEFAULT_TIME_AIR;

        // Camera constructor to set up initial values
        Camera(int width, int height, glm::vec3 position);

        // Updates and exports the camera matrix to the Vertex Shader
        void Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform);

        void jump();
        void land();
        void fall();

};
#endif