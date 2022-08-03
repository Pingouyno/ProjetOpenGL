#include"../headers/World.h"

World::World()
{
	this->camera = new Camera(screenWidth, screenHeight, glm::vec3(0.0f, 0.0f, 0.2f));
    this->entities = {};
    setup3DShapes();

    //Pour blend les endroits vides des png
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    this->gameOverlay = new Overlay(camera, WorldState::GAME);
    this->menuOverlay = new Overlay(camera, WorldState::MENU);
}

void World::render()
{
    glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	renderActive3DShapes();

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

//méthodes privées

void World::setup3DShapes()
{
	Texture* deux_png = new Texture("resources/textures/deux_icon.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	(*deux_png).texUnit(*shaderProgram3D, "tex0", 0);

	Texture* grass_png = new Texture("resources/textures/grass.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	(*grass_png).texUnit(*shaderProgram3D, "tex0", 0);

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
					addShape(new Quad(pos, wallSize, deux_png, Quad::Axis::Y));

				if (x == 0 || (x == LAB_SIZE && z != LAB_SIZE - 1)) 
					addShape(new Quad(pos, wallSize, grass_png, Quad::Axis::X));
			}

			if (z % LAB_SIZE == 0 && x != LAB_SIZE) 
				addShape(new Quad(pos, wallSize, grass_png, Quad::Axis::Z));

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