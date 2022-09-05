#include"../headers/EventManager.h"


EventManager::EventManager(World* world)
{
    this->world = world;
    this->camera = world->player->camera;
	this->player = world->player;
	this->mousePicker = new MousePicker(camera);
}

void EventManager::doEntityPhysics()
{
	for (Entity* ptrEntity : world->loadedEntities)
	{
		//vérifier que la destination ne mène pas dans un chunk non chargé (sinon on tombe hors du monde lorsque chunk unload)
		if (world->getChunkAt(ptrEntity->getPotentialNewPos()) != nullptr)
		{
			ptrEntity->previousRawVelocity = ptrEntity->velocity;
			ptrEntity->collisionLog = vec3(0);
			world->checkEntityCollidingAnyCube(ptrEntity);
			if (world->isAnyColliding(ptrEntity->collisionLog))
			{
				if (ptrEntity->collisionLog.x != 0) ptrEntity->setVelocity(ptrEntity->velocity - vec3(ptrEntity->velocity.x, 0, 0));
				if (ptrEntity->collisionLog.y != 0) ptrEntity->setVelocity(ptrEntity->velocity - vec3(0, ptrEntity->velocity.y, 0));
				if (ptrEntity->collisionLog.z != 0) ptrEntity->setVelocity(ptrEntity->velocity - vec3(0, 0, ptrEntity->velocity.z));
			}
			ptrEntity->moveToVelocity();
		}
		else ptrEntity->resetVelocity();
	}
}

void EventManager::Inputs(GLFWwindow* window)
{
	camera->updateMousePos(window);
	checkKeyboardCamMovement(window);
	checkMouseCamMovement(window);

    checkKeyboardEvents(window);
	checkMouseEvents(window);

	checkMoveAndPhysics(window);
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


void EventManager::checkMouseCamMovement(GLFWwindow* window)
{
	//ex : si on vient de sortit du menuOverlay
	if (shouldCenterCursor)
	{
		camera->centerMouseOnWindow(window);
		shouldCenterCursor = false;
	}

	//Gère le mouvement de caméra ou clics via souris, x = vertical et y = horizontal
	//ajuste l'orientation de la caméra sur le curseur, et met à jour les coordonnées de la caméra
	if (worldState == WorldState::GAME)
	{
		// Stores the coordinates of the cursor
		double mouseX = camera->mousePos.x;
		double mouseY = camera->mousePos.y;
		// Fetches the coordinates of the cursor

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
		camera->centerMouseOnWindow(window);
	}
}


void EventManager::checkKeyboardEvents(GLFWwindow* window)
{
	//**TOUCHE T**______________________
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE && !waitingForTPress)
	{
		waitingForTPress = true;
	//passer du mode menu au mode jeu
	}else if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && waitingForTPress)
	{
		waitingForTPress = false;
		world->deselectTextBox();
		worldState = worldState == WorldState::GAME ? WorldState::MENU : WorldState::GAME;
		if (worldState == WorldState::MENU) 
		{
			// Unhides cursor since camera is not looking around anymore
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			// Makes sure the next time the camera looks around it doesn't jump
		}else 
		{
			shouldCenterCursor = true;
			// Hides mouse cursor
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}
	}

	//**TOUCHE 1**______________________
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE && !waitingFor1Press)
	{
		waitingFor1Press = true;
	//passer du mode menu au mode jeu
	}else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && waitingFor1Press)
	{
		waitingFor1Press = false;
		if (worldState == WorldState::GAME) 
		{
			world->setHeldItemSlot(0);
		}
	}

	//**TOUCHE 2**______________________
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE && !waitingFor1Press)
	{
		waitingFor2Press = true;
	//passer du mode menu au mode jeu
	}else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && waitingFor2Press)
	{
		waitingFor2Press = false;
		if (worldState == WorldState::GAME) 
		{
			world->setHeldItemSlot(1);
		}
	}
}


void EventManager::checkMouseEvents(GLFWwindow* window)
{
	//met à jour le ray de la caméra
	mousePicker->update();

	//**CLICK GAUCHE**_________________________________________________________________
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && !waitingForLClick)
	{
		waitingForLClick = true;
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && waitingForLClick)
	{
		//CLICK GAUCHE
		waitingForLClick = false;

		if (worldState == WorldState::MENU)
		{
			//détecter la collision avec les overlays
			glm::vec3 mousePos(camera->mousePos.x, camera->mousePos.y, 0.0f);
			world->checkCameraCollidingAnyOverlay(mousePos);
		}
		else if (worldState == WorldState::GAME)
		{
			
			Block* lookedBlock = world->getFirstBlockCollidingWithRay(world->player->getPos(), mousePicker->currentRay);
			if (lookedBlock != nullptr)
			{
				//important de mettre avant car la texture de lookedBlock change
				if (gameMode != GameMode::CREATIVE)
				{
					world->addEntityItem(new EntityItem(lookedBlock->pos, lookedBlock->tex));
				}
				world->despawnBlockAt(lookedBlock->pos);
			}
			
		}
	}

	//**CLICK DROIT**_________________________________________________________________
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE && !waitingForRClick)
	{
		waitingForRClick = true;
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && waitingForRClick)
	{
		waitingForRClick = false;

		if (worldState == WorldState::GAME)
		{
			Block* lookedBlock = world->getFirstBlockCollidingWithRay(world->player->getPos(), mousePicker->currentRay);
			if (lookedBlock != nullptr)
			{
				vec3 adjacentPos = world->getPosAdjacentToLookedFace(lookedBlock, world->player->getPos(), mousePicker->currentRay);
				Cube3D* adjacentBlock = world->getBlockAt(adjacentPos);
				
				if (adjacentBlock != nullptr) 
					
					//s'assurer que le bloc n'entre en collision avec aucune autre entité
					if (adjacentBlock->isCollidingEntity(world->player->getPos(), world->player->hitBoxDimensions)) 
						return;
					for (Entity* e : world->loadedEntities)
						if (adjacentBlock->isCollidingEntity(e->getPos(), e->hitBoxDimensions))
							return;
					world->spawnBlockAt(adjacentPos, world->player->textureInHand);
					//world->spawnTreeAt(adjacentPos);
			}
		}
	}
}

void EventManager::checkMoveAndPhysics(GLFWwindow* window)
{
	const bool wasTouchingGround = player->isTouchingGround();
	vec3 newVelocity(0);

	//VITESSE
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		newVelocity += player->maxSpeed * -camera->Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && (gameMode == CREATIVE || wasTouchingGround))
	{
		if (gameMode == GameMode::CREATIVE) player->maxSpeed = Player::FLYING_FAST_SPEED;
		else player->maxSpeed = Player::FAST_SPEED;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		player->maxSpeed = Player::NORMAL_SPEED;
	}

	//si on n'est pas en créatif alors bouger lentement si on est dans le ciel
	if (gameMode != GameMode::CREATIVE && !wasTouchingGround)
	{
		player->maxSpeed = Player::FLYING_SURVIVAL_SPEED;
	}

	// inputs de mouvement physique
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		//On prend les coordonnées x, y, z de l'orientation et on retire le y (pour ne pas monter/descendre). 
		//On trouve le vecteur normal au vecteur normal, soit on retourne à la direction pointéer

		if (gameMode == GameMode::CREATIVE) newVelocity += player->maxSpeed * camera->Orientation;
		else newVelocity += player->maxSpeed * -glm::normalize(glm::cross(glm::vec3(-camera->Orientation.z, camera->Orientation.y, camera->Orientation.x), camera->Up));
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		newVelocity += player->maxSpeed * -glm::normalize(glm::cross(camera->Orientation, camera->Up));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		if (gameMode == GameMode::CREATIVE) newVelocity += player->maxSpeed * -camera->Orientation;
		else newVelocity += player->maxSpeed * glm::normalize(glm::cross(glm::vec3(-camera->Orientation.z, camera->Orientation.y, camera->Orientation.x), camera->Up));
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		newVelocity += player->maxSpeed * glm::normalize(glm::cross(camera->Orientation, camera->Up));
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		if (gameMode == CREATIVE) newVelocity += player->maxSpeed * camera->Up;
		else if (player->collisionLog.y != 0 && player->previousRawVelocity.y < 0)
		{
			//équivalent de jump()
			newVelocity += Entity::JUMPING_VELOCITY;
			PlaySound::playJumpSound();
		}
	}

	if (gameMode != GameMode::CREATIVE && !wasTouchingGround)
	{
		player->addVelocityCapped(newVelocity);
	}else
	{
		player->setVelocity(newVelocity);
	}

	//appliquer la gravité, important de mettre avant le test de saut, pour savoir si on tombe ou non
	if (gameMode != GameMode::CREATIVE) 
	{
		player->addVelocity(Entity::FALLING_VELOCITY);
	}

	//vérifier évènement avec entités items
	for (EntityItem* item : world->entityItems)
	{
		if (player->isColliding(item) && !item->isAttackImmune)
		{
			world->player->textureInHand = item->itemCube->tex;
			world->removeEntityItem(item);
			break;
		}
	}

	//mettre à jour avant de modifier la vélocité
	player->previousRawVelocity = player->velocity;
	
	player->collisionLog = vec3(0);
    world->checkEntityCollidingAnyShape(player);
	if (world->isAnyColliding(player->collisionLog))
	{
		if (player->collisionLog.x != 0) player->setVelocity(player->velocity - vec3(player->velocity.x, 0, 0));
		if (player->collisionLog.y != 0) player->setVelocity(player->velocity - vec3(0, player->velocity.y, 0));
		if (player->collisionLog.z != 0) player->setVelocity(player->velocity - vec3(0, 0, player->velocity.z));
	}
	player->moveToVelocity();
} 

