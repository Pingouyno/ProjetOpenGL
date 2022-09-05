#include"../headers/Camera.h"

float Camera::VERT_PAD_SENSITIVITY = 2.0f;
float Camera::HORI_PAD_SENSITIVITY = 2.8f;

Camera::Camera(int width, int height, vec3* position)
{  
	Camera::width = width;
	Camera::height = height;
	this->Position = position;
	projectionMatrix = mat4(1.0f);
	viewMatrix = mat4(1.0f);
}

//met à jour les matrices et les shaders selon la position (et orientation de la caméra)
void Camera::Matrix(float FOVdeg, float nearPlane, float farPlane)
{
	// Initializes matrices since otherwise they will be the null matrix
	viewMatrix = glm::lookAt(*Position, *Position + Orientation, Up);
	projectionMatrix = glm::mat4(1.0f);
	
	// Adds perspective to the scene
	projectionMatrix = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

	shaderProgramCube->Activate();
	//la variable camMatrix pour le shader cube également
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramCube->ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(projectionMatrix * viewMatrix));

	shaderProgram3D->Activate();
	// Exports the camera matrix to the Vertex Shader
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram3D->ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(projectionMatrix * viewMatrix));
}

void Camera::updateMousePos(GLFWwindow* window)
{
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	mousePos = vec2((float)x, (float)y);
}

void Camera::centerMouseOnWindow(GLFWwindow* window)
{
	glfwSetCursorPos(window, (screenWidth / 2), (screenHeight / 2));
	updateMousePos(window);
}