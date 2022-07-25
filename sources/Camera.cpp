#include"../headers/Camera.h"
#include"../headers/Shape.h"

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

void Camera::checkCamMovement(GLFWwindow* window)
{
	Shape* prevCollidingShapePtr = Shape::checkCameraCollidingAnyShape(*this);
	//inverser les mouvements de direction si on est en collision 
	
	glm::vec3 previousPosition = Position;

	// Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		//On prend les coordonnées x, y, z de l'orientation et on retire le y (pour ne pas monter/descendre). 
		//On trouve le vecteur normal au vecteur normal, soit on retourne à la direction pointéer

		if (isInCreative) Position += speed * fact * Orientation;
		else Position += speed * fact * -glm::normalize(glm::cross(glm::vec3(-Orientation.z, Orientation.y, Orientation.x), Up));
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Position += speed * fact * -glm::normalize(glm::cross(Orientation, Up));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		if (isInCreative) Position += speed * fact * -Orientation;
		else Position += speed * fact * glm::normalize(glm::cross(glm::vec3(-Orientation.z, Orientation.y, Orientation.x), Up));
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Position += speed * fact * glm::normalize(glm::cross(Orientation, Up));
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		if (isInCreative) Position += speed * fact * Up;
		else jump();
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		Position += speed * fact * -Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && isInCreative || !isInAir)
	{
		speed = FAST_SPEED;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		speed = NORMAL_SPEED;
	}

	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		isInCreative = true;
	}else if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
	{
		isInCreative = false;
		fall();
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
	

	//logique pour vérifier si on doit atterir pour bloquer le clipping, tout en glissant sur la forme
	Shape* newCollidingShapePtr = Shape::checkCameraCollidingAnyShape(*this);
	glm::vec3 newPosition = Position;
	if (prevCollidingShapePtr == nullptr && newCollidingShapePtr != nullptr)
	{
		/*Nous avons des nouvelles coordonnées x, y, z. Si on est en collision, alors on essaie de restaurer
		 chaque coordonnée individuellement pour voir si l'on ne devient plus en colision (originalPosition)
		 Si c'est le cas pour y alors on sait qu'on a touché une plateforme au sol. permet de glisser en collision*/
		Position.y = previousPosition.y;
		if (!(*newCollidingShapePtr).isColliding(*this)) 
		{
			land();
		}else 
		{
			Position.x = previousPosition.x;
			if ((*newCollidingShapePtr).isColliding(*this)) 
				Position.z = previousPosition.z;
			
			/*on sait que le y n'est pas impliqué dans la collision, alors on est dans le ciel. 
			on peut accepter la mise à jour de y.Toutefois, sur une forme bizarre, il POURRAIT y avoir à nouveau une collision ??? */
			Position.y = newPosition.y;

			//TODO: potentiellement retirer/remettre ce IF
			//if ((*newCollidingShapePtr).isColliding(*this)) 
			//	Position.y = previousPosition.y;
		}
	}else if (!isInCreative && !isInAir && newCollidingShapePtr == nullptr)
	{
		//vérifier si l'on est dans le vide. si oui, commencer à tomber
		Position.y -= timeInAir * JUMP_FALL_ACCELERATION;
		if (Shape::checkCameraCollidingAnyShape(*this) == nullptr)
		{
			fall();
		}else
		{
			Position.y = newPosition.y;
		}
	}

}

void Camera::checkMouseMovement(GLFWwindow* window)
{
	//Gère le mouvement de caméra via clavier, x = vertical et y = horizontal
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

	// Handles mouse inputs
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		// Hides mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

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
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		// Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Makes sure the next time the camera looks around it doesn't jump
		firstClick = true;
	}
}

void Camera::jump()
{
	if (!isInAir){
		isInAir = true;
		//parce que l'on va passer la moitié du temps en ascenscion (négatif) et l'autre  moitié en ascension
		timeInAir = -JUMP_LENGTH / 2.0f;
	}
}

void Camera::land()
{
	isInAir = false;	
	timeInAir = 0.0f;
}

//commencer à tomber si l'on tombe d'un bord
void Camera::fall()
{
	isInAir = true;
	timeInAir = 0.0f;	
}