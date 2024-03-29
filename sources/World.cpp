#include"../headers/World.h"

const int World::PLAYER_RANGE = 11;
const float World::COLLISION_PRECISION = 50.0f;

World::World()
{
	const float spawnPos = 100;
	this->player = new Player(glm::vec3(spawnPos, Chunk::getPerlinHeightOf(spawnPos, spawnPos) + Shape::camBoxHeight, spawnPos));
	this->camera = this->player->camera;
    setupEntities();

    setup3DShapes();

	setupChunks();
	
	this->score = 0;
    //Pour blend les endroits vides des png
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    this->gameOverlay = new GameOverlay(camera);
    this->menuOverlay = new MenuOverlay(camera);

	//important car sinon c'est NULL dans Player
	setHeldItemSlot(0);
}

void World::doEntityBehaviors()
{	
	skyBox->moveTo(player->getPos());
	checkEntityTimerDespawns();
	checkEntityDeaths();
	checkEntityDeathDespawns();
	player->checkEndOfAttackImmuneTimer();
	
	for (Entity* ptrEntity : loadedEntities)
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
	glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
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
	for (Entity* ptrEntity : loadedEntities)
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
	for (Entity* ptrEntity : loadedEntities)
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
				blockCounts[i] = ((Block*)textureVec[i])->getIndiceCount();
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

//vérifie s'il faut ajouter l'entité dans loaded ou unloaded
void World::addEntity(Entity* entity)
{
	if (getChunkAt(entity->getPos()) != nullptr)
	{
		loadedEntities.push_back(entity);
	}
	else
	{
		unloadedEntities.push_back(entity);
	}
}

//stocke l'entité dans un vecteur à part, pour calcul du timer de despawn
void World::addEntityItem(EntityItem* entityItem)
{
	this->entityItems.push_back(entityItem);
	addEntity(entityItem);
}

//remove et delete une entité
void World::killEntity(Entity* entity)
{
	bool found = false;
	for (int i = 0 ; i < loadedEntities.size() ; i++)
	{
		if (loadedEntities[i] == entity)
		{
			loadedEntities[i] = loadedEntities.back();
			loadedEntities.pop_back();
			found = true;
			break;
		}
	}
	if (!found) for (int i = 0 ; i < unloadedEntities.size() ; i++)
	{
		if (unloadedEntities[i] == entity)
		{
			unloadedEntities[i] = unloadedEntities.back();
			unloadedEntities.pop_back();
			break;
		}
	}

	entity->Delete();
}

void World::killEntityItem(EntityItem* entityItem)
{
	for (int i = 0 ; i < entityItems.size() ; i++)
	{
		if (entityItems[i] == entityItem)
		{
			entityItems[i] = entityItems.back();
			entityItems.pop_back();
			break;
		}
	}
	entityItem->die();
}

//Trouve la première entité qui entre en collision avec le rayon du joueur, max range = PLAYER_RANGE
Entity* World::getFirstEntityCollidingWithRay(vec3 startingPos, vec3 ray)
{
	//normalisé
	const vec3 oneRayStep = ray / COLLISION_PRECISION;

	vec3 currentRayPos = startingPos;
	int i = 0;
	while (i < PLAYER_RANGE * COLLISION_PRECISION)
	{
		currentRayPos += oneRayStep;
		for (Entity* e : loadedEntities)
		{
			if (e->isColliding(currentRayPos))
			{
				return e;
			}
		}
		i++;
	}
	return nullptr;
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
			if (currentBlock != nullptr && !currentBlock->isAir() && currentBlock->isColliding(currentRayPos)) 
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

//vérifie les formes ainsi que les cubes
vec3 World::checkEntityCollidingAnyShape(Entity* entity)
{
	vec3 &oldPos = entity->getPos();
	vec3 newPos = entity->getPotentialNewPos();
	vec3 &entityDimensions = entity->hitBoxDimensions;

    for (Shape* ptrShape : shapes)
    {
        if (ptrShape->active && ptrShape->isCollidingEntity(newPos, entityDimensions))
            ptrShape->reportCollisionWithEntity(player->collisionLog, oldPos, newPos, entityDimensions);
    }

	for (Quad* ptrShape : worldBorders)
    {
        if (ptrShape->active && ptrShape->isCollidingEntity(newPos, entityDimensions))
            ptrShape->reportCollisionWithEntity(player->collisionLog, oldPos, newPos, entityDimensions);
    }

	checkEntityCollidingAnyCube(player);

    return vec3(player->collisionLog);
}

//retourne un collisionLog
vec3 World::checkEntityCollidingAnyCube(Entity* entity)
{
	//reset le collisionLog de l'entité
    vec3 &collisionLog = entity->collisionLog;

	//trouver les blocs près du joueur
	vec3 entityPos = glm::round(entity->getPos());

	vector<Block*> nearbyBlocks({});
	for (float x = -1 ; x <= 1 ; x++)
		for (float y = -1 ; y <= 1 ; y++)
			for (float z = -1 ; z <= 1 ; z++)
				nearbyBlocks.push_back(getBlockAt(entity->getPotentialNewPos() + vec3(x*Shape::ROT_X + y*Shape::ROT_Y + z*Shape::ROT_Z)));
			
	for (int i = 0 ; i < nearbyBlocks.size() ; i++)
	{
		Block* nearbyBlock = nearbyBlocks[i];
		if (nearbyBlock != nullptr && !nearbyBlock->isAir() && entity->wouldThenBeCollidingCube(entity->velocity, nearbyBlock))
		{
			entity->reportCollisionWithCubeThen(nearbyBlock);
		}
	}
    return vec3(collisionLog);
}

bool World::isAnyColliding(vec3 &collisionLog)
{
    return collisionLog.x != 0 || collisionLog.y != 0 || collisionLog.z != 0;
}

//méthode lente, qui met à jour le terrain autour. utiliser pendant le runtime
void World::spawnBlockAt(vec3 pos, Texture* tex)
{	
	Block* spawnedBlock = getBlockAt(pos);
	if (spawnedBlock != nullptr)
	{
		if(spawnedBlock->isInRenderingVec())
		{
			removeBlockFromRendering(spawnedBlock);
		}
		spawnedBlock->tex = tex;
		updateBlock(spawnedBlock);
	}else
	{	
		//vérifier si le bloc serait compris dans le monde, et si oui, mette ce bloc dans la file d'attente
		if (pos.x >= 0 && pos.x < WORLD_SIZE 
		   && pos.y >= 0 && pos.y < CHUNK_HEIGHT
		   && pos.z >= 0 && pos.z < WORLD_SIZE)
		{
			BlockToSpawn blockToSpawn;
			blockToSpawn.positionToSpawnAt = pos;
			blockToSpawn.textureToSpawn = tex;
			blocksToSpawn.push_back(blockToSpawn);
		}
	}
}

void World::despawnBlockAt(vec3 pos)
{	
	Block* despawnedBlock = getBlockAt(pos);
	if (despawnedBlock != nullptr)
	{
		//important de le faire avant de changer la texture, sinon renderingVec bugge
		if (despawnedBlock->isInRenderingVec()) removeBlockFromRendering(despawnedBlock);
		despawnedBlock->tex = Texture::Air;
		updateBlock(despawnedBlock);
	}
}

void World::checkEntityTimerDespawns()
{
	for (int i = 0 ; i < entityItems.size() ; i++)
	{
		if (entityItems[i]->framesUntilDespawn == 0)
		{
			killEntityItem(entityItems[i]);
			//car killEntityItems va diminuer la taille du tableau
			i--;
		}
	}
}

//débute l'animation et rend death->true pour toutes les entités qui n'ont plus de vies.
void World::checkEntityDeaths()
{
	for (Entity* e : loadedEntities)
	{
		if (!e->isDead && e->health <= 0)
		{
			e->die();
		}
	}
}

//despawn toutes les entités mortes lorsque leur timer d'invincibilité est écoulé
void World::checkEntityDeathDespawns()
{
	for (Entity* e : loadedEntities)
	{
		if (e->isDead && !e->isAttackImmune)
		{
			killEntity(e);
		}
	}
}

/*retourne le chunk avec un calcul de la position sur la taille d'un chunk
 fonctionne SEULEMENT pour les positions positives
 ACCEPTE les y < 0 et y >= 64*/
Chunk* World::getChunkAt(vec3 pos)
{
	if (pos.x < 0 || pos.x >= WORLD_SIZE 
	   || pos.z < 0 || pos.z >= WORLD_SIZE) 
		return nullptr;
	return chunkMat[pos.x / CHUNK_SIZE][pos.z / CHUNK_SIZE];
}

//retourne le bloc du chunk qui contient celui-ci. on calcule en divisant la position par la taille d'un chunk
Block* World::getBlockAt(vec3 pos)
{
	Chunk* chunk = getChunkAt(pos);
	if (chunk != nullptr)
	{
		Block* block = chunk->getBlockAt(pos);
		if (block != nullptr)
		{
			return block;
		}
	}
	return nullptr;
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

void World::deselectTextBox()
{    
	if (selectedTextBox != nullptr) ((TextEntity2D*)selectedTextBox)->deselect();
}

void World::setHeldItemSlot(int slot)
{	gameOverlay->setActiveHotBarSlot(slot);
	player->textureInHand = gameOverlay->getTextureFromSlot(slot);
}

/*
render les chunks proches du joueur (dist <= CHUNK_VIEW_DISTANCE),
delete les chunks loins du joueur (dist == CHUNK_VIEW_DISTANCE + 1),
fait une seule opération par appel (pour ne pas réduire le framerate)
*/
void World::updateChunks()
{
	vector<vec3> chunkPositionsToLoad = {};
	vector<Chunk*> chunksToUnload = {};

	vec3 currentPos = player->getPos() - vec3(CHUNK_RENDER_DISTANCE * CHUNK_SIZE, 0, CHUNK_RENDER_DISTANCE * CHUNK_SIZE);
	currentPos = Chunk::getNearestFloorChunkPosOf(currentPos);

	for (int x = -CHUNK_RENDER_DISTANCE ; x <= CHUNK_RENDER_DISTANCE; x++)
	{
		for (int z = -CHUNK_RENDER_DISTANCE ; z <= CHUNK_RENDER_DISTANCE ; z++)
		{
			//vérifier que l'on est pas sur le bord du monde
			if (currentPos.x >= 0 && currentPos.x < WORLD_SIZE 
			   && currentPos.z >= 0 && currentPos.z < WORLD_SIZE)
			{		
				//seulement loader s'il ne reste rien à unloaded
				if (getChunkAt(currentPos) == nullptr)
				{
					chunkPositionsToLoad.push_back(currentPos);
				}
					
			}
			currentPos.z += CHUNK_SIZE;
		}
		currentPos.z -= (2*(CHUNK_RENDER_DISTANCE) + 1) * CHUNK_SIZE;
		currentPos.x += CHUNK_SIZE;
	}

	for (Chunk* chunk : loadedChunks)
	{
		const vec3 chunkDistBetweenPlayer = chunk->getDistanceInChunksBetween(player->getPos());

		if (chunkDistBetweenPlayer.x > CHUNK_RENDER_DISTANCE 
	       || chunkDistBetweenPlayer.z > CHUNK_RENDER_DISTANCE)
		{
			chunksToUnload.push_back(chunk);
		}
	}

	//pour nombres aléatoires
	std::random_device dev;
    std::mt19937 rng(dev());

	//unloader un chunk s'il loader un chunk s'il ne reste rien à unload, sinon loader un chunk si l'on a trouvé
	if (chunksToUnload.size() > 0)
	{
		//trouver nombre aléatoire dans le range
		std::uniform_int_distribution<std::mt19937::result_type> dist(0, chunksToUnload.size() - 1); 
		int randInt = dist(rng);


		unloadChunk(chunksToUnload[randInt]);
		
	}else if (chunkPositionsToLoad.size() > 0)
	{
		std::uniform_int_distribution<std::mt19937::result_type> dist(0, chunkPositionsToLoad.size() - 1); 
		int randInt = dist(rng);

		Chunk* newChunk = new Chunk(chunkPositionsToLoad[randInt]);
		loadChunk(newChunk);
		setupTrees(newChunk);
	}
}

void World::spawnTreeAt(vec3 pos)
{
	std::random_device dev;
   	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> distHeight(5, 7); 
	int height = distHeight(rng);

	//former une boule autour du bloc le plus haut
	std::uniform_int_distribution<std::mt19937::result_type> distLeaves(1, 2); 
	vec3 topPos = pos + vec3(0, height - 1, 0);
	for (int diffx = -2 ; diffx <= 2 ; diffx++)
	{
		for (int diffy = -2 ; diffy <= 2 ; diffy++)
		{
			for (int diffz = -2 ; diffz <= 2 ; diffz++)
			{	
				if (((abs(diffx) == 2) + (abs(diffy) == 2) + (abs(diffz) == 2)) >= 2) continue;

				if( (diffx == 2 || diffx == -2 || diffy == 2 || diffy == -2 || diffz == 2 || diffz == -2) 
				   && distLeaves(rng) == 1) continue;

				//ici on ne n'utilise pas spawnBlockAt() car ce n'est pas performant ; on évite d'utiliser 
				spawnBlockAt(topPos + vec3(diffx, diffy, diffz), Texture::Leaves);
			}
		}
	}

	for (int i = 0 ; i < height ; i++)
	{
		spawnBlockAt(pos, Texture::Wood);
		pos.y += 1;
	}
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

/*un block "air" est un bloc tex == Texture::Air
si le bloc adjacent n'est pas chargé, on regarde s'il serait de l'air, en fonction de l'algorithme de génération de terrain.*/
bool World::isBlockNearAir(Block* block)
{
	vec3 nearbyPositions[] = 
	{
		block->pos + Shape::ROT_Y,
		block->pos - Shape::ROT_Y,
		block->pos + Shape::ROT_X,
		block->pos - Shape::ROT_X,
		block->pos + Shape::ROT_Z,
		block->pos - Shape::ROT_Z
	};

	for (int i = 0 ; i < sizeof(nearbyPositions) / sizeof(nearbyPositions[0]) ; i++)
	{
		Block* nearbyBlock = getBlockAt(nearbyPositions[i]);
		if (nearbyBlock == nullptr)
		{
			if (Chunk::wouldBlockBeAirAt(nearbyPositions[i])) return true;
		}else
		{
			if (nearbyBlock->isAir()) return true;
		}
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

	if (!block->isAir())
	{
		//vérifier qu'il est près d'air pour savoir s'il faut render
		if (!block->isInRenderingVec() && isBlockNearAir(block))
		{
			addBlockToRendering(block);
		}

		//puisqu'on a ajouté un bloc, vérifier s'il faut retirer les blocs proches du rendering
		for (int i = 0 ; i < sizeof(nearbyBlocks) / sizeof(block) ; i++)
		{
			if (nearbyBlocks[i] != nullptr && !nearbyBlocks[i]->isAir() && nearbyBlocks[i]->isInRenderingVec() && !isBlockNearAir(nearbyBlocks[i]))
			{
				removeBlockFromRendering(nearbyBlocks[i]);
			}
		}
	}
	//sinon updater tous les cubes autour
	else
	{	
		Block* nearbyBlock;
		for (int i = 0 ; i < sizeof(nearbyBlocks) / sizeof(block) ; i++)
		{
			nearbyBlock = nearbyBlocks[i];
			if (nearbyBlock != nullptr && !nearbyBlock->isAir())
			{
				if (!nearbyBlock->isInRenderingVec()) addBlockToRendering(nearbyBlock);
			}
		}
	}
}

void World::addBlockToRendering(Block* block)
{
	if(block->isInRenderingVec())
	{
		removeBlockFromRendering(block);
	}

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

/*ajoute tous les blocs du chunks au rendering s'il correspondent aux critères.
*EDGE CASE : render les blocs près des bords, car le chunk voisin n'est pas loadé donc un ne sait pas si c'est de l'air*/
void World::setupBlocksToRender(Chunk* chunk)
{
	for (auto &vecX : chunk->blockMat)
	{
		for (auto &vecY : vecX)
		{
			for (Block* block : vecY)
			{
				if (!block->isAir() && isBlockNearAir(block))
				{
					addBlockToRendering(block);
				}
			}
		}
	}
}

void World::setupEntities()
{
	entityItems = {};
	unloadedEntities = {};

	vec3 pos(player->getPos().x, NAN, player->getPos().z);
	pos.y = Chunk::getPerlinHeightOf(pos.x, pos.z) + 2;
	Snowman* firstSnowman = new Snowman(pos, player);
	unloadedEntities.push_back(firstSnowman);

	Snowman* lastSnowman = firstSnowman;
	Snowman* newSnowman;
	
	for (int i = 0 ; i < 25 ; i++)
	{
		vec3 pos = lastSnowman->getPos() + vec3(2, 0, 0);
		pos.y = Chunk::getPerlinHeightOf(pos.x, pos.z) + 2;
		newSnowman = new Snowman(pos);
		unloadedEntities.push_back(newSnowman);
		lastSnowman = newSnowman;
	}

	for (int i = 0 ; i < 25 ; i++)
	{
		vec3 pos = lastSnowman->getPos() + vec3(2, 0, 0);
		pos.y = Chunk::getPerlinHeightOf(pos.x, pos.z) + 2;
		newSnowman = new Snowman(pos, unloadedEntities[i + 1]);
		unloadedEntities.push_back(newSnowman);
		lastSnowman = newSnowman;
	}
}

void World::setup3DShapes()
{
	//La taille n'est pas importante car le shader tracera comme si il est à la distance maximale
	this->skyBox = new Cube3D(vec3(0), vec3(1), Texture::get3DImgTexture(Texture::SKYBOX));
	skyBox->setToBackground();

	//bords de monde
	worldBorders = 
	{
		new Quad(vec3(-Block::BLOCK_SIZE / 2, CHUNK_HEIGHT / 2, WORLD_SIZE / 2), WORLD_SIZE + Block::BLOCK_SIZE, Texture::get2DImgTexture("sadge.png"), Quad::Axis::X),
		new Quad(vec3(-Block::BLOCK_SIZE / 2 + WORLD_SIZE, CHUNK_HEIGHT / 2, WORLD_SIZE / 2), WORLD_SIZE + Block::BLOCK_SIZE, Texture::get2DImgTexture("sadge.png"), Quad::Axis::X),
		new Quad(vec3(WORLD_SIZE / 2, CHUNK_HEIGHT / 2, -Block::BLOCK_SIZE / 2), WORLD_SIZE + Block::BLOCK_SIZE, Texture::get2DImgTexture("sadge.png"), Quad::Axis::Z),
		new Quad(vec3(WORLD_SIZE / 2, CHUNK_HEIGHT / 2, -Block::BLOCK_SIZE / 2 + WORLD_SIZE), WORLD_SIZE + Block::BLOCK_SIZE, Texture::get2DImgTexture("sadge.png"), Quad::Axis::Z),
	};
}

/*réserve les matrices de blocks et de chunks, puis load tous les chunks
  load tous les chunks plutôt qu'un seul par seconde ;
  charge tous les chunks, PUIS calcule quels blocs loader*/
void World::setupChunks()
{
	loadedChunks = {};

	//initialiser la matrice de rendering selon le nombre de texture qui existent
    blocksToRenderMat = {};
	for (int i = 0 ; i < Texture::tex3DNames.size(); i++)
	{
		blocksToRenderMat.push_back({});
	}

	//initialiser la matrice de chunks selon la taille du monde
	chunkMat = vector<vector<Chunk*>>(WORLD_CHUNK_COUNT);
	for (int x = 0 ; x < WORLD_CHUNK_COUNT; x++)
	{
		chunkMat[x] = vector<Chunk*>(WORLD_CHUNK_COUNT);
		for (int z = 0 ; z < WORLD_CHUNK_COUNT; z++)
		{
			chunkMat[x][z] = nullptr;
		}
	}
	
	//loader tous les chunks (version modifiée de updateChunks(0)
	vec3 currentPos = player->getPos() - vec3((CHUNK_RENDER_DISTANCE) * CHUNK_SIZE, 0, (CHUNK_RENDER_DISTANCE) * CHUNK_SIZE);
	currentPos = vec3((int)currentPos.x - (int)currentPos.x % CHUNK_SIZE, 0, (int)currentPos.z - (int)currentPos.z % CHUNK_SIZE);

	for (int x = -(CHUNK_RENDER_DISTANCE) ; x <= (CHUNK_RENDER_DISTANCE); x++)
	{
		for (int z = -(CHUNK_RENDER_DISTANCE) ; z <= (CHUNK_RENDER_DISTANCE); z++)
		{
			//vérifier que l'on est pas sur le bord du monde
			if (currentPos.x >= 0 && currentPos.x < WORLD_SIZE 
			   && currentPos.z >= 0 && currentPos.z < WORLD_SIZE)
			{
				if (getChunkAt(currentPos) == nullptr)
				{
					loadChunk(new Chunk(currentPos));
				}
			}

			currentPos.z += CHUNK_SIZE;
		}
		currentPos.z -= (2*(CHUNK_RENDER_DISTANCE) + 1) * CHUNK_SIZE;
		currentPos.x += CHUNK_SIZE;
	}

	for (Chunk* chunk : loadedChunks)
	{
		setupTrees(chunk);
	}
}

void World::loadChunk(Chunk* chunk)
{
	chunkMat[chunk->indXInMat][chunk->indZInMat] = chunk;
	loadedChunks.push_back(chunk);
	setupBlocksToRender(chunk);
	spawnBlocksAvailableInNewChunk(chunk);

	//vérifier le chargement de nouvelles entités
	Entity* unloadedEntity;
	for (int i = 0 ; i < unloadedEntities.size() ; i++)
	{
		unloadedEntity = unloadedEntities[i];
		if (chunk->isPosWithinThisChunk(unloadedEntity->getPos()))
		{				
			unloadedEntities[i] = unloadedEntities.back();
			unloadedEntities.pop_back();
			i--;

			loadedEntities.push_back(unloadedEntity);
		}
	}
}

void World::unloadChunk(Chunk* chunk)
{
	chunkMat[chunk->indXInMat][chunk->indZInMat] = nullptr;
	for (int i = 0 ; i < loadedChunks.size() ; i++)
	{
		if (loadedChunks[i] == chunk)
		{
			loadedChunks[i] = loadedChunks.back();
			loadedChunks.pop_back();
			i--;
			break;
		}
	}
	for (auto &vecX : chunk->blockMat)
	{
		for (auto &vecY : vecX)
		{
			for (Block* block : vecY)
			{
				if (block->isInRenderingVec())
				{
					removeBlockFromRendering(block);
				}
				delete(block);
			}
		}
	}

	//vérifier si on doit dé-charger des entités
	Entity* loadedEntity;
	for (int i = 0 ; i < loadedEntities.size() ; i++)
	{
		loadedEntity = loadedEntities[i];
		if (chunk->isPosWithinThisChunk(loadedEntity->getPos()))
		{				
			loadedEntities[i] = loadedEntities.back();
			loadedEntities.pop_back();
			i--;

			unloadedEntities.push_back(loadedEntity);
		}
	}

	delete(chunk);
}

//Génère aléatoirement un/des arbre(s) sur un chunk
void World::setupTrees(Chunk* chunk)
{
	//probabiilité de 1/x par chunk
	const int TREE_SPAWN_PROBABILITY = 3;

	std::random_device dev;
   	std::mt19937 rng(dev());

	std::uniform_int_distribution<std::mt19937::result_type> distSpawnTree(1, TREE_SPAWN_PROBABILITY); 
	std::uniform_int_distribution<std::mt19937::result_type> distCoord(0, CHUNK_SIZE - 1); 

	if (distSpawnTree(rng) == 1)
	{
		int x = chunk->chunkPos.x + distCoord(rng);
		int z = chunk->chunkPos.z + distCoord(rng);
		int y = Chunk::getPerlinHeightOf(x, z) + 1;

		spawnTreeAt(vec3(x, y, z));
	}
}

void World::spawnBlocksAvailableInNewChunk(Chunk* chunk)
{
	//spawner les blocs qui peuvent désormais être chargés. 
	for (int i = 0 ; i < blocksToSpawn.size() ; i++)
	{
		BlockToSpawn &blockInfo = blocksToSpawn[i];
		if (chunk->isPosWithinThisChunk(blockInfo.positionToSpawnAt))
		{
			spawnBlockAt(blockInfo.positionToSpawnAt, blockInfo.textureToSpawn);
			blocksToSpawn[i] = blocksToSpawn.back();
			blocksToSpawn.pop_back();
			i--;
		}
	}
}