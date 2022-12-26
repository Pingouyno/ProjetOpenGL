#include"../headers/Chunk.h"

PerlinNoise* Chunk::perlinNoise = new PerlinNoise();

Chunk::Chunk(vec3 chunkPos)
{
    this->chunkPos = chunkPos;
	this->indXInMat = chunkPos.x / CHUNK_SIZE;
	this->indZInMat = chunkPos.z / CHUNK_SIZE;

    //initialiser la matrice des blocs
	blockMat.resize(CHUNK_SIZE);
	for (int x = 0 ; x < CHUNK_SIZE ; x++)
	{
		blockMat[x].resize(CHUNK_HEIGHT);
		for (int y = 0 ; y < CHUNK_HEIGHT; y++)
		{
			blockMat[x][y].resize(CHUNK_SIZE);
		}
	}

    setupBlocks();
}

void Chunk::setupBlocks()
{
	vec3 pos = chunkPos;

	for (int x = chunkPos.x ; x < chunkPos.x + CHUNK_SIZE ; x++)
	{
		for (int z = chunkPos.z ; z < chunkPos.z + CHUNK_SIZE ; z++)
		{
			float perlinHeight = getPerlinHeightOf(x, z);

			//mettre y jusqu'en bas (0)
			vec3 currentPos = pos;

			for (int y = chunkPos.y ; y < chunkPos.y + CHUNK_HEIGHT ; y++)
			{
				addBlock(new Block(currentPos, getBlockTextureToGenerateAt(currentPos, perlinHeight)));
				currentPos.y += Block::BLOCK_SIZE;
			}

			pos.z += Block::BLOCK_SIZE;
		}
		pos.x += Block::BLOCK_SIZE;
		pos.z -= Block::BLOCK_SIZE * CHUNK_SIZE;
	}
}

Block* Chunk::getBlockAt(vec3 pos)
{
    if (pos.x < chunkPos.x || pos.x >= chunkPos.x + CHUNK_SIZE 
	   || pos.z < chunkPos.z || pos.z >= chunkPos.z + CHUNK_SIZE 
	   || pos.y < chunkPos.y || pos.y >= chunkPos.y + CHUNK_HEIGHT) 
	    return nullptr;
	return blockMat[pos.x - chunkPos.x][pos.y - chunkPos.y][pos.z - chunkPos.z];
}

vec3 Chunk::getDistanceInChunksBetween(vec3 otherPos)
{
	return Chunk::getNearestFloorChunkPosOf(abs(this->chunkPos - otherPos)) / (float)CHUNK_SIZE;
}

bool Chunk::isPosWithinThisChunk(vec3 testedPos)
{
	const vec3 diffVec = testedPos - chunkPos;
	return diffVec.x >= 0 && diffVec.x < CHUNK_SIZE
	   && diffVec.y >= 0 && diffVec.y < CHUNK_HEIGHT
	   && diffVec.z >= 0 && diffVec.z < CHUNK_SIZE;
}

//return true si l'algorithme de génération de terrain donnerait un bloc d'air
bool Chunk::wouldBlockBeAirAt(vec3 &blockPos)
{
	return getBlockTextureToGenerateAt(blockPos, getPerlinHeightOf(blockPos.x, blockPos.z)) == Texture::Air;
}

float Chunk::getPerlinHeightOf(float x, float z)
{
	const float perlinOut = perlinNoise->noise((double)x/WORLD_SIZE, 1, (double)z/WORLD_SIZE);
	const float perlinHeight = std::round(CHUNK_HEIGHT * perlinOut);
	return perlinHeight;
}

//arrondit la position au chunk le plus proche (pour spawning)
vec3 Chunk::getNearestFloorChunkPosOf(vec3 posToRound)
{
	return vec3((int)posToRound.x - (int)posToRound.x % CHUNK_SIZE, 0, (int)posToRound.z - (int)posToRound.z % CHUNK_SIZE);
}

//algorithme de génération de terrain
Texture* Chunk::getBlockTextureToGenerateAt(vec3 &targetPos, float perlinHeight)
{
	if (targetPos.y < perlinHeight) 
	{
		if (targetPos.y < 3) return Texture::Bedrock;
		else if (targetPos.y < 25) return Texture::Stone;
		else return Texture::Dirt;
	}
	else if (targetPos.y == perlinHeight) 
	{
		if (targetPos.y < 25) return Texture::Stone;
		else return Texture::Grass;
	}
	else if  (targetPos.y > perlinHeight)
	{
		return Texture::Air;
	}

	cout << "\n\n**ERREUR : getBlockTextureToGenerateAt() n'a rien trouvé!**\n\n";
	return nullptr;
}

void Chunk::addBlock(Block* block)
{
    blockMat[block->pos.x - chunkPos.x][block->pos.y - chunkPos.y][block->pos.z - chunkPos.z] = block;  
}

