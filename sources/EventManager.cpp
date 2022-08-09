#include"../headers/EventManager.h"


EventManager::EventManager(World* world)
{
    this->world = world;
    this->camera = world->camera;
}


void EventManager::Inputs(GLFWwindow* window)
{
    checkKeyEvents(window);
	checkMouseEvents(window);

	//potentiellement à retirer au besoin et performance
	checkKeyboardCamMovement(window);
}


void EventManager::checkKeyEvents(GLFWwindow* window)
{

	//vérifier si l'on a appuyé sur le bouton de menu
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE && !waitingForTPress)
	{
		waitingForTPress = true;
	//passer du mode menu au mode jeu
	}else if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && waitingForTPress)
	{
		//inverser l'état de jeu
		waitingForTPress = false;
		worldState = worldState == WorldState::GAME ? WorldState::MENU : WorldState::GAME;
		if (worldState == WorldState::MENU) 
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

	glm::vec3 previousPosition = camera->Position;

	// inputs de mouvement physique
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		//On prend les coordonnées x, y, z de l'orientation et on retire le y (pour ne pas monter/descendre). 
		//On trouve le vecteur normal au vecteur normal, soit on retourne à la direction pointéer

		if (gameMode == GameMode::CREATIVE) camera->Position += camera->speed * camera->Orientation;
		else camera->Position += camera->speed * -glm::normalize(glm::cross(glm::vec3(-camera->Orientation.z, camera->Orientation.y, camera->Orientation.x), camera->Up));
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera->Position += camera->speed * -glm::normalize(glm::cross(camera->Orientation, camera->Up));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		if (gameMode == GameMode::CREATIVE) camera->Position += camera->speed * -camera->Orientation;
		else camera->Position += camera->speed * glm::normalize(glm::cross(glm::vec3(-camera->Orientation.z, camera->Orientation.y, camera->Orientation.x), camera->Up));
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera->Position += camera->speed * glm::normalize(glm::cross(camera->Orientation, camera->Up));
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		if (gameMode == CREATIVE) camera->Position += camera->speed * camera->Up;
		else if (!camera->isInAir)
		{
			camera->jump();
			PlaySound::playJumpSound();
			world->incrementScore(1);
		}
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		camera->Position += camera->speed * -camera->Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && (gameMode == CREATIVE || !camera->isInAir))
	{
		camera->speed = Camera::FAST_SPEED;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		camera->speed = Camera::NORMAL_SPEED;
	}

	//appliquer la gravité
	if (!gameMode == GameMode::CREATIVE) 
	{
		if (camera->isInAir)
		{
			camera->timeInAir += 1.0f;
		}
		camera->Position.y -= camera->timeInAir * Camera::JUMP_FALL_ACCELERATION;
	}
	

	//calculer les collisions et permettre d'atterir, tomber et de glisser sur les murs
	glm::vec3 newPosition = camera->Position;
	vector<int> collisionLog = world->checkCameraCollidingAnyShape(previousPosition, newPosition);

	if (world->isAnyColliding(collisionLog))
	{
		if (collisionLog[0] != 0) camera->Position.x -= newPosition.x - previousPosition.x;

		if (collisionLog[1] != 0) {
			if (gameMode != CREATIVE && camera->isInAir) camera->land();
			camera->Position.y = previousPosition.y;
		}else if (!camera->isInAir) camera->fall();

		if (collisionLog[2] != 0) camera->Position.z -= newPosition.z - previousPosition.z;

	}else if (!camera->isInAir) camera->fall();

	/*
	 cout << "\ncollision : " << collisionLog[0] << " | "<<collisionLog[1] << " | " << collisionLog[2];
	 cout << "\nprev : " << previouscamera->Position.x << " | "<< previouscamera->Position.y << " | " << previouscamera->Position.z;
	 cout << "\n now : " << camera->Position.x << " | "<< camera->Position.y << " | " << camera->Position.z << "\n";
	*/

} 

void EventManager::checkMouseEvents(GLFWwindow* window)
{
	//Gère le mouvement de caméra ou clics via souris, x = vertical et y = horizontal
	//bouger la souris

	if (worldState == WorldState::MENU) {
		//regarder s'il y a eu clic de souris
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && !waitingForLClick)
		{
			waitingForLClick = true;
		}else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && waitingForLClick)
		{
			waitingForLClick = false;

			//détecter la collision avec les overlays
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			glm::vec3 mousePos((float)x, (float)y, 0.0f);
			world->checkCameraCollidingAnyOverlay(mousePos);
		}
	}else
	{
		//sinon puisqu'on n'est pas dans le menu, faire bouger la caméra
		// Prevents camera from jumping on the first click
		if (firstClick)
		{
			glfwSetCursorPos(window, (screenWidth / 2), (screenHeight / 2));
			firstClick = false;
		}

		// Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		// and then "transforms" them into degrees 
		float rotX = camera->sensitivity * (float)(mouseY - (screenHeight / 2)) / screenHeight;
		float rotY = camera->sensitivity * (float)(mouseX - (screenWidth / 2)) / screenWidth;

		// Calculates upcoming vertical change in the Orientation
		glm::vec3 newOrientation = glm::rotate(camera->Orientation, glm::radians(-rotX), glm::normalize(glm::cross(camera->Orientation, camera->Up)));

		// Decides whether or not the next vertical Orientation is legal or not
		if (abs(glm::angle(newOrientation, camera->Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			camera->Orientation = newOrientation;
		}

		// Rotates the Orientation left and right
		camera->Orientation = glm::rotate(camera->Orientation, glm::radians(-rotY), camera->Up);

		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(window, (screenWidth / 2), (screenHeight / 2));
	}
	
}

void EventManager::checkKeyboardCamMovement(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
        glm::vec3 newOrient = glm::rotate(camera->Orientation, glm::radians(camera->VERT_PAD_SENSITIVITY), glm::normalize(glm::cross(camera->Orientation, camera->Up)));

		if (abs(glm::angle(newOrient, camera->Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			camera->Orientation = newOrient;
		}
	}
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
        camera->Orientation = glm::rotate(camera->Orientation, glm::radians(camera->HORI_PAD_SENSITIVITY), camera->Up);
	}
        //Gère le mouvement de caméra via clavier, b = x et g = y
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
        glm::vec3 newOrient = glm::rotate(camera->Orientation, glm::radians(-camera->VERT_PAD_SENSITIVITY), glm::normalize(glm::cross(camera->Orientation, camera->Up)));
		if (abs(glm::angle(newOrient, camera->Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			camera->Orientation = newOrient;
		}
	}
        //Gère le mouvement de caméra via clavier, b = x et g = y
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
        camera->Orientation = glm::rotate(camera->Orientation, glm::radians(-camera->HORI_PAD_SENSITIVITY), camera->Up);
	}
}


