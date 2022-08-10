#include"../headers/World.h"

World::World()
{
	this->player = new Player(glm::vec3(0.0f, 0.0f, 0.2f));
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
    glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	renderActive3DShapes();
	renderActiveEntities();

    shaderProgram2D -> Activate();
    glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	World::renderOverlays();

    shouldReloadArrays = false;
}


//Render toutes les entités, et désactive "newShapeCreated"
void World::renderActive3DShapes()
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

void World::renderOverlays()
{
    gameOverlay->render();
    if (worldState == WorldState::MENU)
	{
		menuOverlay->render();
	}
}

void World::checkCameraCollidingAnyOverlay(glm::vec3 &mousePos)
{
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
        if ((*ptrShape).active && (*ptrShape).isColliding(newPos))
            (*ptrShape).reportCollision(collisionLog, oldPos, newPos);
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

//fin fonctions dynamiques______________________________________________________________

//méthodes privées

void World::setupEntities()
{
	Snowman* firstSnowman = new Snowman(glm::vec3(10, 10, 10), player);

	entities = 
	{
		firstSnowman,
	};

	Snowman* lastSnowman = firstSnowman;
	Snowman* newSnowman = nullptr;
	//ajouter un train de snowmans
	
	for (int i = 0 ; i < 50 ; i++)
	{
		newSnowman = new Snowman(vec3(lastSnowman->getPos() + vec3(10.0f, 0, 12.0f)), lastSnowman);
		entities.push_back(newSnowman);
		lastSnowman = newSnowman;
	}
	
}

void World::setup3DShapes()
{
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
}