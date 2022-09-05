#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include<vector>
#include<fstream>
using namespace std;

#include"../libraries/include/GLAD/glad.h"
#include"../libraries/include/glfw/glfw3.h"
#include"../libraries/include/glm/gtc/matrix_transform.hpp"
#include"../libraries/include/glm/gtc/type_ptr.hpp"
#include"../libraries/include/glm/gtx/rotate_vector.hpp"
#include"../libraries/include/glm/gtx/vector_angle.hpp"
using namespace glm;

#include"shaderClass.h"
#include"Globals.h"

class Camera
{
    private:
        //référence vers pos du joueur
        glm::vec3* Position;

    public:
        static float VERT_PAD_SENSITIVITY;
        static float HORI_PAD_SENSITIVITY;
    
        glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec2 mousePos;

        float width;
        float height;

        mat4 projectionMatrix;
        mat4 viewMatrix;

        float sensitivity = 100.0f;

        // Camera constructor to set up initial values
        Camera(int width, int height, vec3* position);

        // Updates and exports the camera matrix to the Vertex Shader
        void Matrix(float FOVdeg, float nearPlane, float farPlane);

        void updateMousePos(GLFWwindow* window);
        void centerMouseOnWindow(GLFWwindow* window);

};
#endif