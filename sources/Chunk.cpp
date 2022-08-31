#include"../headers/Chunk.h"

PerlinNoise* Chunk::perlinNoise = new PerlinNoise();

Chunk::Chunk(vec3 chunkPos)
{
    this->chunkPos = chunkPos;
	this->isUnloading = false;
	this->isLoaded = false;
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
				if (currentPos.y < perlinHeight) 
				{
					if (currentPos.y < 3) addBlock(new Block(currentPos, Texture::get3DImgTexture(Texture::TEX3D::BEDROCK)));
					else if (currentPos.y < 25) addBlock(new Block(currentPos, Texture::get3DImgTexture(Texture::TEX3D::STONE)));
					else addBlock(new Block(currentPos, Texture::get3DImgTexture(Texture::TEX3D::DIRT)));
				}
				else if (currentPos.y == perlinHeight) 
				{
					if (currentPos.y < 25) addBlock(new Block(currentPos, Texture::get3DImgTexture(Texture::TEX3D::STONE)));
					else addBlock(new Block(currentPos, Texture::get3DImgTexture(Texture::TEX3D::GRASS)));
				}
				else if  (currentPos.y > perlinHeight)
				{
					Block* addedBlock = new Block(currentPos, Texture::get3DImgTexture(Texture::TEX3D::EARTH));
					addedBlock->active = false;
					addBlock(addedBlock);
				}
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

//return true si l'algorithme de génération de terrain donnerait un bloc d'air
bool Chunk::wouldBlockBeAirAt(vec3 &blockPos)
{
	return blockPos.y > getPerlinHeightOf(blockPos.x, blockPos.z);
}

float Chunk::getPerlinHeightOf(float x, float z)
{
	const float perlinOut = perlinNoise->noise((double)x/CHUNK_SIZE, 1, (double)z/WORLD_SIZE);
	//float perlinOut = perlinNoise->noise((double)x/WORLD_SIZE, 1, (double)z/WORLD_SIZE);
	const float perlinHeight = std::round(CHUNK_HEIGHT * perlinOut);
	return perlinHeight;
}

void Chunk::addBlock(Block* block)
{
    blockMat[block->pos.x - chunkPos.x][block->pos.y - chunkPos.y][block->pos.z - chunkPos.z] = block;  
}