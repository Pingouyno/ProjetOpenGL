#include"../headers/Camera.h"
#include"../headers/Shape.h"
#include"../headers/PlaySound.h"

#include<fstream>

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

void Camera::Inputs(GLFWwindow* window)
{
	checkCamMovement(window);
	checkMouseMovement(window);
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

void Camera::checkCamMovement(GLFWwindow* window)
{
	glm::vec3 previousPosition = Position;

	// Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		//On prend les coordonnées x, y, z de l'orientation et on retire le y (pour ne pas monter/descendre). 
		//On trouve le vecteur normal au vecteur normal, soit on retourne à la direction pointéer

		if (isInCreative) Position += speed * Orientation;
		else Position += speed * -glm::normalize(glm::cross(glm::vec3(-Orientation.z, Orientation.y, Orientation.x), Up));
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Position += speed * -glm::normalize(glm::cross(Orientation, Up));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		if (isInCreative) Position += speed * -Orientation;
		else Position += speed * glm::normalize(glm::cross(glm::vec3(-Orientation.z, Orientation.y, Orientation.x), Up));
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Position += speed * glm::normalize(glm::cross(Orientation, Up));
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		if (isInCreative) Position += speed * Up;
		else if (!isInAir)
		{
			jump();
			PlaySound::playJumpSound();
		}

	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		Position += speed * -Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && (isInCreative || !isInAir))
	{
		speed = FAST_SPEED;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		speed = NORMAL_SPEED;
	}

	//appliquer la gravité
	if (!isInCreative) 
	{
		if (isInAir)
		{
			timeInAir += 1.0f;
		}
		Position.y -= timeInAir * JUMP_FALL_ACCELERATION;
	}
	

	//calculer les collisions et permettre d'atterir, tomber et de glisser sur les murs
	glm::vec3 newPosition = Position;
	vector<int> collisionLog = Shape::checkCameraCollidingAnyShape(previousPosition, newPosition);

	if (Shape::isAnyColliding(collisionLog))
	{
		if (collisionLog[0] != 0) Position.x -= newPosition.x - previousPosition.x;

		if (collisionLog[1] != 0) {
			if (!isInCreative && isInAir) land();
			Position.y = previousPosition.y;
		}else if (!isInAir) fall();

		if (collisionLog[2] != 0) Position.z -= newPosition.z - previousPosition.z;

	}else if (!isInAir) fall();

	/*
	 cout << "\ncollision : " << collisionLog[0] << " | "<<collisionLog[1] << " | " << collisionLog[2];
	 cout << "\nprev : " << previousPosition.x << " | "<< previousPosition.y << " | " << previousPosition.z;
	 cout << "\n now : " << Position.x << " | "<< Position.y << " | " << Position.z << "\n";
	*/

}

void Camera::checkMouseMovement(GLFWwindow* window)
{
	//Gère le mouvement de caméra via clavier, x = vertical et y = horizontal
	//checkKeyMouseMovement(window);

	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE && !waitingForTPress)
	{
		waitingForTPress = true;
	//passer du mode menu au mode jeu
	}else if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && waitingForTPress)
	{
		waitingForTPress = false;
		isInMenu = !isInMenu;
		if (isInMenu) 
		{
			// Unhides cursor since camera is not looking around anymore
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			// Makes sure the next time the camera looks around it doesn't jump
			firstClick = true;
		}else 
		{
			// Hides mouse cursor
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}
	}
	
	//bouger la souris
	if (!isInMenu) {
		// Prevents camera from jumping on the first click
		if (firstClick)
		{
			glfwSetCursorPos(window, (width / 2), (height / 2));
			firstClick = false;
		}

		// Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		// and then "transforms" them into degrees 
		float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
		float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

		// Calculates upcoming vertical change in the Orientation
		glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

		// Decides whether or not the next vertical Orientation is legal or not
		if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			Orientation = newOrientation;
		}

		// Rotates the Orientation left and right
		Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(window, (width / 2), (height / 2));
	}

	//regarder s'il y a eu clic de souris
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && !waitingForLClick)
	{
		waitingForLClick = true;
	}else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && waitingForLClick)
	{
		waitingForLClick = false;

		if (isInMenu)
		{
			//détecter la collision avec les formes 2D
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			glm::vec3 mousePos((float)x, (float)y, 0.0f);
			Shape::checkCameraCollidingAnyHUD(mousePos);
		}
	}
	
}

void Camera::checkKeyboardMouseMovement(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
        glm::vec3 newOrient = glm::rotate(Orientation, glm::radians(VERT_PAD_SENSITIVITY), glm::normalize(glm::cross(Orientation, Up)));

		if (abs(glm::angle(newOrient, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			Orientation = newOrient;
		}
	}
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
        Orientation = glm::rotate(Orientation, glm::radians(HORI_PAD_SENSITIVITY), Up);
	}
        //Gère le mouvement de caméra via clavier, b = x et g = y
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
        glm::vec3 newOrient = glm::rotate(Orientation, glm::radians(-VERT_PAD_SENSITIVITY), glm::normalize(glm::cross(Orientation, Up)));
		if (abs(glm::angle(newOrient, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			Orientation = newOrient;
		}
	}
        //Gère le mouvement de caméra via clavier, b = x et g = y
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
        Orientation = glm::rotate(Orientation, glm::radians(-HORI_PAD_SENSITIVITY), Up);
	}
}
