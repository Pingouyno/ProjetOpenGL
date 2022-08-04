#include"../headers/Camera.h"


float Camera::NORMAL_SPEED = 0.10f;
float Camera::FAST_SPEED = 2 * NORMAL_SPEED;
float Camera::VERT_PAD_SENSITIVITY = 2.0f;
float Camera::HORI_PAD_SENSITIVITY = 2.8f;
//longueur du saut (en frames)
float Camera::JUMP_LENGTH = 45.0f;
//force G qui nous donne x * 1u/s d'accélération
float Camera::JUMP_FALL_ACCELERATION = 9.8 * 1.0f / 1830.0f; 

Camera::Camera(int width, int height, glm::vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Position = position;
}

void Camera::Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform)
{
	// Initializes matrices since otherwise they will be the null matrix
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// Makes camera look in the right direction from the right position
	view = glm::lookAt(Position, Position + Orientation, Up);
	// Adds perspective to the scene
	projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

	// Exports the camera matrix to the Vertex Shader
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
}

void Camera::jump()
{
	isInAir = true;
	//parce que l'on va passer la moitié du temps en ascenscion (négatif) et l'autre  moitié en ascension
	timeInAir = -JUMP_LENGTH / 2.0f;
}

//on met le temps dans l'air à 5.0 pour éviter de mini-sauts
void Camera::land()
{
	isInAir = false;	
	timeInAir = DEFAULT_TIME_AIR;
}

//commencer à tomber si l'on tombe d'un bord
void Camera::fall()
{
	isInAir = true;
	timeInAir = DEFAULT_TIME_AIR;	
}
