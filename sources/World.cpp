#include"../headers/World.h"

const int World::PLAYER_RANGE = 11;
const float World::COLLISION_PRECISION = 50.0f;
const int World::WORLD_SIZE = 384; //384; //12 chunks * 16 blocks par chunk
const int World::WORLD_HEIGHT = 64.0f;
const int World::CHUNK_SIZE = WORLD_SIZE;

World::World()
{
	this->perlinNoise = new PerlinNoise();
	this->player = new Player(glm::vec3(8.0f, 40.0f, 8.0f));
	this->camera = this->player->camera;
    setupEntities();
	blocksToRenderMat = {};
	for (int i = 0 ; i < Texture::tex3DNames.size(); i++)
	{
		blocksToRenderMat.push_back({});
	}
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
	VAO1->Bind();

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
	renderBlocks();

	//render les formes
	shaderProgram3D->Activate();
	renderActiveShapes();
	renderActiveEntities();

	//render les overlays
    shaderProgram2D->Activate();
    glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	renderOverlays();
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

void World::renderBlocks()
{
	for (auto &textureVec : blocksToRenderMat)
	{
		if (textureVec.size() != 0)
		{
			void* blockStarts[textureVec.size()];
			GLsizei blockCounts[textureVec.size()];

			for (int i = 0 ; i < textureVec.size() ; i++)
			{
				blockStarts[i] = (void*)(textureVec[i]->indexInIndices * sizeof(int));
				blockCounts[i] = textureVec[i]->getIndiceCount();
			}
			textureVec[0]->tex->Bind();

			glMultiDrawElements(GL_TRIANGLES, blockCounts, GL_UNSIGNED_INT, blockStarts, textureVec.size());

			textureVec[0]->tex->Unbind();
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

//trouve la première forme qui entre en collisions avec le rayon* 
Block* World::getFirstBlockCollidingWithRay(vec3 startingPos, vec3 ray)
{
	const vec3 oneRayStep = ray / COLLISION_PRECISION;

	vec3 currentRayPos = startingPos;
	vec3 lastRoundedPos = vec3(NAN);
	vec3 newRoundedPos;
	Block* currentBlock;

	int i = 0;
	while (i < PLAYER_RANGE)
	{
		currentRayPos += oneRayStep;
		newRoundedPos = glm::round(currentRayPos);

		if (newRoundedPos != lastRoundedPos)
		{
			currentBlock = getBlockAt(newRoundedPos);
			if (currentBlock != nullptr && currentBlock->active && currentBlock->isColliding(currentRayPos)) 
			{
				return currentBlock;
			}
			i++;
		}
		lastRoundedPos = newRoundedPos;
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
            ptrShape->reportCollisionWithHuman(collisionLog, oldPos, newPos);
    }

	//trouver les blocs près du joueur
	vec3 playerPos = glm::round(player->getPos());
	playerPos.y -= 1;

	vector<Block*> nearbyBlocks({});
	for (float x = -1 ; x <= 1 ; x++)
		for (float y = -1 ; y <= 1 ; y++)
			for (float z = -1 ; z <= 1 ; z++)
				nearbyBlocks.push_back(getBlockAt(playerPos + vec3(x*Shape::ROT_X + y*Shape::ROT_Y + z*Shape::ROT_Z)));
			
	for (int i = 0 ; i < nearbyBlocks.size() ; i++)
	{
		if (nearbyBlocks[i] != nullptr && nearbyBlocks[i]->active && nearbyBlocks[i]->isCollidingHuman(newPos))
		{
			nearbyBlocks[i]->reportCollisionWithHuman(collisionLog, oldPos, newPos);
		}
	}
	
    return collisionLog;
}

bool World::isAnyColliding(vector<int> &collisionLog)
{
    return collisionLog[0] != 0 || collisionLog[1] != 0 || collisionLog[2] != 0;
}

void World::spawnBlockAt(vec3 pos, Texture* tex)
{	
	Block* spawnedBlock = getBlockAt(pos);
	if (spawnedBlock != nullptr)
	{
		spawnedBlock->tex = tex;
		spawnedBlock->active = true;
		updateBlock(spawnedBlock);
	}
}

void World::despawnBlockAt(vec3 pos)
{	
	Block* despawnedBlock = getBlockAt(pos);
	if (despawnedBlock != nullptr)
	{
		despawnedBlock->active = false;
		updateBlock(despawnedBlock);
	}
}

Block* World::getBlockAt(vec3 pos)
{
	if (pos.x < 0 || pos.x >= WORLD_SIZE 
	   || pos.z < 0 || pos.z >= WORLD_SIZE 
	   || pos.y < 0 || pos.y >= WORLD_HEIGHT) 
	    return nullptr;
	return blockMat[pos.x][pos.y][pos.z];
}

//donne le adjacent à la fae sur laquelle le joueur regarde
vec3 World::getPosAdjacentToLookedFace(Block* block, vec3 raySource, vec3 ray)
{	

	const vec3 oneRayStep = ray / COLLISION_PRECISION;
	vec3 currentRayPos = raySource;

	for (int i = 0; i < PLAYER_RANGE * COLLISION_PRECISION; i++)
	{
		currentRayPos += oneRayStep;

		if (block->isColliding(currentRayPos))
		{
			break;
		}
	}

	vec3 collisionPoint = glm::normalize(currentRayPos - block->pos);
	vec3 diff = glm::abs(collisionPoint);

	vec3 adjacentCoord = vec3(NAN);
	if (diff.x >= diff.y && diff.x >= diff.z) adjacentCoord = Shape::AXIS_X;
	else if (diff.y >= diff.x && diff.y >= diff.z) adjacentCoord = Shape::AXIS_Y;
	else if (diff.z >= diff.x && diff.z >= diff.y) adjacentCoord = Shape::AXIS_Z;

	vec3 direction = collisionPoint / diff;  //(1, -1, 1) par exemple
	adjacentCoord = adjacentCoord * direction + block->pos;

	return adjacentCoord;
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

void World::setHeldItemSlot(int slot)
{	gameOverlay->setActiveHotBarSlot(slot);
	player->textureInHand = gameOverlay->getTextureFromSlot(slot);
}

//fin fonctions dynamiques______________________________________________________________

//méthodes privées

void World::addShape(Shape* shape)
{   
    shapes.push_back(shape);    
}

//obligatoire pour les cubes 3D
void World::addCube3D(Cube3D* cube)
{   
    cubes3D.push_back(cube);  
}

//obligatoire pour les cubes 3D
void World::addBlock(Block* block)
{   
	blockMat[block->pos.x][block->pos.y][block->pos.z] = block;   
}

//un block "air" est un bloc active->false
bool World::isBlockNearAir(Block* block)
{
	Block* nearbyBlocks[] = 
	{
		getBlockAt(block->pos + Shape::ROT_Y),
		getBlockAt(block->pos - Shape::ROT_Y),
		getBlockAt(block->pos + Shape::ROT_X),
		getBlockAt(block->pos - Shape::ROT_X),
		getBlockAt(block->pos + Shape::ROT_Z),
		getBlockAt(block->pos - Shape::ROT_Z)
	};

	for (int i = 0 ; i < sizeof(nearbyBlocks) / sizeof(block) ; i++)
	{
		if (nearbyBlocks[i] != nullptr && !nearbyBlocks[i]->active) return true;
	}

	return false;
}

//met à jour le block, soit render le block s'il est près d'air ou de-render s,Il n'est pas à côté d'air
void World::updateBlock(Block* block)
{	
	Block* nearbyBlocks[] = 
	{
		getBlockAt(block->pos + Shape::ROT_Y),
		getBlockAt(block->pos - Shape::ROT_Y),
		getBlockAt(block->pos + Shape::ROT_X),
		getBlockAt(block->pos - Shape::ROT_X),
		getBlockAt(block->pos + Shape::ROT_Z),
		getBlockAt(block->pos - Shape::ROT_Z)
	};

	//si block courant actif (donc pas de l'air)
	if (block->active)
	{
		//vérifier qu'il est près d'air pour savoir s'il faut render
		if (!block->isInRenderingVec() && isBlockNearAir(block))
		{
			addBlockToRendering(block);
		}

		//puisqu'on a ajouté un bloc, vérifier s'il faut retirer les blocs proches du rendering
		for (int i = 0 ; i < sizeof(nearbyBlocks) / sizeof(block) ; i++)
		{
			if (nearbyBlocks[i] != nullptr && nearbyBlocks[i]->active && nearbyBlocks[i]->isInRenderingVec() && !isBlockNearAir(nearbyBlocks[i]))
			{
				removeBlockFromRendering(nearbyBlocks[i]);
			}
		}
	}
	//sinon le cube n'est pas actif donc updater tous les cubes autour
	else
	{	
		//retirer le cube courant   //TODO : remplacer par quelque-chose qui ne va pas laisser de nullptr dans le tableau
		if (block->isInRenderingVec()) removeBlockFromRendering(block);
		Block* nearbyBlock;
		//ajouter les adjacents actifs dans le tableau à render    //TODO : remplacer par quelque-chose qui ne va pas dupliquer 
		for (int i = 0 ; i < sizeof(nearbyBlocks) / sizeof(block) ; i++)
		{
			nearbyBlock = nearbyBlocks[i];
			if (nearbyBlock != nullptr && nearbyBlock->active)
			{
				if (!nearbyBlock->isInRenderingVec()) addBlockToRendering(nearbyBlock);
			}
		}
	}
}

void World::addBlockToRendering(Block* block)
{
	vector<Block*> &textureVec = blocksToRenderMat[block->tex->tex3Did];
	block->indexInRendering = textureVec.size();
	textureVec.push_back(block);

	GLBufferManager::mallocBlockIntoGLBuffer(block);
	block->generate();
}

//retirer efficacement un objet de l'array, en échangeant sa position avec le dernier élément et en supprimant le dernier indice
void World::removeBlockFromRendering(Block* blockToRemove)
{
	vector<Block*> &textureVec = blocksToRenderMat[blockToRemove->tex->tex3Did];
	Block* lastBlock = textureVec.back();
	
	textureVec[blockToRemove->indexInRendering] = lastBlock;
	lastBlock->indexInRendering = blockToRemove->indexInRendering;
	blockToRemove->indexInRendering = -1;

	textureVec.pop_back();

	GLBufferManager::freeBlockFromGLBuffer(blockToRemove);
}

void World::setupBlocksToRender()
{
	for (auto &vecX : blockMat)
	{
		for (auto &vecY : vecX)
		{
			for (Block* block : vecY)
			{
				if (block->active && isBlockNearAir(block))
				{
					addBlockToRendering(block);
				}
			}
		}
	}
}

void World::setupEntities()
{
	Snowman* firstSnowman = new Snowman(glm::vec3(10, 10, 10), player);

	entities = 
	{
		firstSnowman,
	};

	//ajouter un train de snowmans	

	firstSnowman->targetEntity = firstSnowman;
}

void World::setup3DShapes()
{
	//La taille n'est pas importante car le shader tracera comme si il est à la distance maximale
	this->skyBox = new Cube3D(vec3(0), vec3(1), Texture::get3DImgTexture(Texture::FIELD));
	skyBox->setToBackground();

	const vec3 defaultPos(0, 0, 0);
	vec3 pos = defaultPos;

	//initialiser la matrice des blocs
	blockMat.resize(CHUNK_SIZE);
	for (int x = 0 ; x < CHUNK_SIZE ; x++)
	{
		blockMat[x].resize(WORLD_HEIGHT);
		for (int y = 0 ; y < WORLD_HEIGHT ; y++)
		{
			blockMat[x][y].resize(CHUNK_SIZE);
		}
	}

	for (int x = 0 ; x < CHUNK_SIZE ; x++)
	{
		for (int z = 0 ; z < CHUNK_SIZE ; z++)
		{
			float perlinOut = perlinNoise->noise((double)x/WORLD_SIZE, 1, (double)z/WORLD_HEIGHT);
			float perlinHeight = std::round(WORLD_HEIGHT * perlinOut);

			//mettre y jusqu'en bas (0)
			vec3 currentPos = pos;

			for (int y = 0 ; y < WORLD_HEIGHT ; y++)
			{
				if (currentPos.y < perlinHeight) 
				{
					if (currentPos.y < 3) addBlock(new Block(currentPos, Texture::get3DImgTexture(Texture::TEX3D::BEDROCK)));
					else if (currentPos.y < 25) addBlock(new Block(currentPos, Texture::get3DImgTexture(Texture::TEX3D::STONE)));
					else addBlock(new Block(currentPos, Texture::get3DImgTexture(Texture::TEX3D::DIRT)));
				}
				else if (currentPos.y == perlinHeight) 
				{
					addBlock(new Block(currentPos, Texture::get3DImgTexture(Texture::TEX3D::GRASS)));
				}
				else if  (currentPos.y > perlinHeight)
				{
					Block* addedBlock = new Block(currentPos, Texture::get3DImgTexture(Texture::TEX3D::EARTH));
					addedBlock->active = false;
					addBlock(addedBlock);
				}
				currentPos.y += Block::BLOCK_SIZE;
			}

			pos.x += Block::BLOCK_SIZE;
		}
		pos.z += Block::BLOCK_SIZE;
		pos.x -= Block::BLOCK_SIZE * CHUNK_SIZE;
	}

	setupBlocksToRender();
}