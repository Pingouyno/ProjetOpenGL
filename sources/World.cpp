#include"../headers/World.h"

World::World()
{
	this->perlinNoise = new PerlinNoise();
	this->player = new Player(glm::vec3(0.0f, 0.0f, 0.0f));
	this->camera = this->player->camera;
    setupEntities();
    setup3DShapes();
	this->score = 0;

    //Pour blend les endroits vides des png
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    this->gameOverlay = new GameOverlay(camera);
    this->menuOverlay = new MenuOverlay(camera);
}

void World::doEntityBehaviors()
{	
	skyBox->moveTo(player->getPos());
	for (Entity* ptrEntity : entities)
	{
		if (ptrEntity->active)
		{
			ptrEntity->doBehavior();
		}
	}
}

void World::render()
{
	shaderProgramCube->Activate();
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	//render la skybox
	glDepthFunc(GL_LEQUAL);
	skyBox->render();
	glDepthFunc(GL_LESS);

	//render les cubeMaps
	renderActive3DCubes();
	renderActive3DCubesEntities();

	//render les formes
	shaderProgram3D->Activate();
	renderActiveShapes();
	renderActiveEntities();

	//render les overlays
    shaderProgram2D->Activate();
    glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	renderOverlays();

    shouldReloadArrays = false;
}

//Render toutes les entités, et désactive "newShapeCreated"
void World::renderActiveShapes()
{
    for (Shape* ptrShape : shapes)
    {
        if ((*ptrShape).active)
        {
            (*ptrShape).render();
        }
    }
}

void World::renderActiveEntities()
{
	for (Entity* ptrEntity : entities)
    {
        if (ptrEntity->active)
        {
            ptrEntity->render();
        }
    }
}

//Render tous les cubes qui ont une texture 3D
void World::renderActive3DCubes()
{
    for (Cube3D* ptrCube : cubes3D)
    {
        if (ptrCube->active)
        {
            ptrCube->render();
        }
    }
}

//TODO : remplir cette fonction
void World::renderActive3DCubesEntities()
{
    
	for (Entity* ptrEntity : entities)
    {
        ptrEntity->render3DCubes();
    }
}

void World::renderOverlays()
{
    gameOverlay->render();
    if (worldState == WorldState::MENU)
	{
		menuOverlay->render();
	}
}

//trouve la première forme qui entre en collisions avec le rayon
Cube3D* World::getFirstCubeCollidingWithRay(vec3 startingPos, vec3 ray)
{
	const int playerRange = 6;
	vec3 currentRayPos = startingPos;
	for (int i = 0 ; i < playerRange ; i++)
	{
		for (Cube3D* cube : cubes3D)
		{
			if (cube->isColliding(currentRayPos)) 
			{
				return cube;
			}
		}
		currentRayPos += ray;
	}
	return nullptr;
}

void World::checkCameraCollidingAnyOverlay(glm::vec3 &mousePos)
{
	TextEntity2D* textBox = ((TextEntity2D*)selectedTextBox);
	if (textBox != nullptr && !textBox->isColliding(mousePos)) 
	{
		textBox->deselect();
	}

    if (worldState == MENU)
	{
		menuOverlay->checkCollisions(mousePos);
	}
}

vector<int> World::checkCameraCollidingAnyShape(glm::vec3 &oldPos, glm::vec3 &newPos)
{
    vector<int> collisionLog({0, 0, 0});
    for (Shape* ptrShape : shapes)
    {
        if (ptrShape->active && ptrShape->isCollidingHuman(newPos))
            (*ptrShape).reportCollisionWithHuman(collisionLog, oldPos, newPos);
    }

	for (Cube3D* ptrCube : cubes3D)
    {
        if (ptrCube->active && ptrCube->isCollidingHuman(newPos))
            ptrCube->reportCollisionWithHuman(collisionLog, oldPos, newPos);
    }
    return collisionLog;
}

bool World::isAnyColliding(vector<int> &collisionLog)
{
    return collisionLog[0] != 0 || collisionLog[1] != 0 || collisionLog[2] != 0;
}

void World::addShape(Shape* shape)
{   
    shapes.push_back(shape);    
}

//obligatoire pour les cubes 3D
void World::addCube3D(Cube3D* cube)
{   
    cubes3D.push_back(cube);    
}

void World::deleteAllShapes()
{
	//supprimer les formes
    int i = shapes.size() - 1;
    while(i >= 0)
    {
        shapes.erase(shapes.begin() + i);
        i--;
    }
    vertices.clear();
    indices.clear();
    shouldReloadArrays = true;
}


//fonctions reliées à la logique de monde dynamique_____________________________________

void World::incrementScore(int amount)
{
	this->score += amount;
	updateScore();
}

void World::updateScore()
{
	gameOverlay->updateScoreDisplay(score);
}

void World::deselectTextBox()
{
	if (selectedTextBox != nullptr) ((TextEntity2D*)selectedTextBox)->deselect();
}

//fin fonctions dynamiques______________________________________________________________

//méthodes privées

void World::setupEntities()
{
	Snowman* firstSnowman = new Snowman(glm::vec3(10, 10, 10), player);

	entities = 
	{
		firstSnowman,
	};

	//ajouter un train de snowmans	

	//firstSnowman->targetEntity = firstSnowman;

	/*
	Snowman* lastSnowman = firstSnowman;
	Snowman* newSnowman = nullptr;
	for (int i = 0 ; i < 50 ; i++)
	{
		newSnowman = new Snowman(vec3(lastSnowman->getPos() + vec3(10.0f, 0, 12.0f)), lastSnowman);
		entities.push_back(newSnowman);
		lastSnowman = newSnowman;
	}
	*/
}

void World::setup3DShapes()
{
	//La taille n'est pas importante car le shader tracera comme si il est à la distance maximale
	this->skyBox = new Cube3D(vec3(0, 0, 0), vec3(1), Texture::get3DImgTexture(Texture::TEX3D::FIELD));
	skyBox->setToBackground();

	const float worldWidth = 16.0f;
	const float worldHeight = 64;
	const vec3 defaultPos(20, worldHeight, 20);
	const float chunkSize = 20.0f;
	const float cubeSize = 1.0f;
	vec3 pos = defaultPos;

	for (int x = 0 ; x < chunkSize ; x++)
	{
		for (int z = 0 ; z < chunkSize ; z++)
		{
			float perlinOut = perlinNoise->noise((double)x/worldWidth, 1, (double)z/worldHeight);
			pos.y = std::round(worldHeight * perlinOut);
			addCube3D(new Cube3D(pos, vec3(cubeSize), Texture::get3DImgTexture(Texture::TEX3D::GRASS)));

			//mettre jusqu'en bas (0)
			vec3 currentPos = pos;
			for (int y = 0 ; y < pos.y ; y++)
			{	
				currentPos.y -= 1;

				/*
				if (currentPos.y < 3) addCube3D(new Cube3D(currentPos, vec3(cubeSize), Texture::get3DImgTexture(Texture::TEX3D::BEDROCK)));
				else if (currentPos.y < 25) addCube3D(new Cube3D(currentPos, vec3(cubeSize), Texture::get3DImgTexture(Texture::TEX3D::STONE)));
				else addCube3D(new Cube3D(currentPos, vec3(cubeSize), Texture::get3DImgTexture(Texture::TEX3D::DIRT)));
				*/

				//optimiser pour ne render que les bords
				if (!(x != 0 && z != 0 && x != chunkSize && z != chunkSize) || y < 2)
				{
					if (currentPos.y < 3) addCube3D(new Cube3D(currentPos, vec3(cubeSize), Texture::get3DImgTexture(Texture::TEX3D::BEDROCK)));
					else if (currentPos.y < 25) addCube3D(new Cube3D(currentPos, vec3(cubeSize), Texture::get3DImgTexture(Texture::TEX3D::STONE)));
					else addCube3D(new Cube3D(currentPos, vec3(cubeSize), Texture::get3DImgTexture(Texture::TEX3D::DIRT)));
				}
			}

			pos.x += cubeSize;
		}

		pos.z += cubeSize;
		pos.x -= cubeSize * chunkSize;
	}

	/*
	Texture* deux_png = Texture::get2DImgTexture("deux_icon.png");
	Texture* grass_png = Texture::get2DImgTexture("grass.png");

	glm::vec3 pos(-10.0f, -10.0f, -10.0f);
	float wallSize = 2.5f;
	int x = 0;

	camera->Position = glm::vec3(pos[0]+wallSize/2, pos[1] + wallSize/1.5, pos[2]+wallSize/2);
	camera->Orientation = glm::rotate(camera->Orientation, glm::radians(240.0f), glm::vec3(0, 1.0f, 0));

	//générer les murs du labyrinthe
	for (int z = 0 ; z < LAB_SIZE + 1; z++)
	{
		for (int x = 0 ; x < LAB_SIZE + 1 ; x++)
		{
			if (z != LAB_SIZE)
			{
				if (x != LAB_SIZE)
					addShape(new Quad(pos - vec3(0, wallSize/2, 0), wallSize, deux_png, Quad::Axis::Y));

				if (x == 0 || (x == LAB_SIZE && z != LAB_SIZE - 1)) 
					addShape(new Quad(pos - vec3(wallSize/2, 0, 0), wallSize, grass_png, Quad::Axis::X));
			}

			if (z % LAB_SIZE == 0 && x != LAB_SIZE) 
				addShape(new Quad(pos - vec3(0, 0, wallSize/2), wallSize, grass_png, Quad::Axis::Z));

			pos[0] += wallSize;

		}

		pos[0] -= (LAB_SIZE + 1) * wallSize;
		pos[2] += wallSize;
	}

	pos = {-10.0f, -10.0f, -10.0f};
	int cpt = 0;
	for (int i = 0 ; i < labyrinth.size(); i++)
	{

		if (labyrinth[i] == 1)
			addShape(new Cube(pos, wallSize, grass_png));
		pos[0] += wallSize;

		cpt++;
		if (cpt == LAB_SIZE)
		{
			cpt = 0;
			pos[0] -= LAB_SIZE * wallSize;
			pos[2] += wallSize;
		}
	}
	*/

	/*
	float cubeSize = 3.0f;
	vec3 default_pos = vec3(-10.0f);

	vec3 pos = default_pos;
	for (int x = 0 ; x < 10 ; x++)
	{
		for (int z = 10 ; z > 0 ; z--)
		{
			addCube3D(new Cube3D(pos, vec3(cubeSize), Texture::get3DImgTexture(Texture::TEX3D::GRASS)));
			pos.z -= cubeSize;
		}
		pos.x += cubeSize;
		pos.z += 10*cubeSize;
	}

	pos = default_pos;
	pos.y -= 1*cubeSize;
	for (int x = 0 ; x < 10 ; x++)
	{
		for (int z = 10 ; z > 0 ; z--)
		{
			addCube3D(new Cube3D(pos, vec3(cubeSize), Texture::get3DImgTexture(Texture::TEX3D::DIRT)));
			pos.z -= cubeSize;
		}
		pos.x += cubeSize;
		pos.z += 10*cubeSize;
	}

	pos = default_pos;
	pos.y -= 2*cubeSize;
	for (int x = 0 ; x < 10 ; x++)
	{
		for (int z = 10 ; z > 0 ; z--)
		{
			addCube3D(new Cube3D(pos, vec3(cubeSize), Texture::get3DImgTexture(Texture::TEX3D::STONE)));
			pos.z -= cubeSize;
		}
		pos.x += cubeSize;
		pos.z += 10*cubeSize;
	}

	pos = default_pos;
	pos.y -= 3*cubeSize;
	for (int x = 0 ; x < 10 ; x++)
	{
		for (int z = 10 ; z > 0 ; z--)
		{
			addCube3D(new Cube3D(pos, vec3(cubeSize), Texture::get3DImgTexture(Texture::TEX3D::BEDROCK)));
			pos.z -= cubeSize;
		}
		pos.x += cubeSize;
		pos.z += 10*cubeSize;
	}
	*/
}