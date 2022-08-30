#ifndef CHUNK_CLASS_H
#define CHUNK_CLASS_H

#include<string>
#include<vector>
using namespace std;

#include"Block.h"
#include"PerlinNoise.h"
#include"Globals.h"

class Chunk
{
    private:
        static PerlinNoise* perlinNoise;

    public:

        //x, y, z
        vector<vector<vector<Block*>>>blockMat;
        vec3 chunkPos;

        Chunk(vec3 chunkPos);

        void setupBlocks();
        Block* getBlockAt(vec3 pos);
    
    private:
        //seulement utilisée pour la GÉNÉRATION de terrain
        void addBlock(Block* block);

};

#endif