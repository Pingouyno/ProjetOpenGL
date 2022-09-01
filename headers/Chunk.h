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
        int indXInMat;
        int indZInMat;
        //x, y, z
        vector<vector<vector<Block*>>>blockMat;
        vec3 chunkPos;

        Chunk(vec3 chunkPos);

        void setupBlocks();
        Block* getBlockAt(vec3 pos);
        vec3 getDistanceInChunksBetween(vec3 otherPos);
    
        static bool wouldBlockBeAirAt(vec3 &blockPos);
        static float getPerlinHeightOf(float x, float z);
        static vec3 getNearestFloorChunkPosOf(vec3 posToRound);
        static Texture* getBlockTextureToGenerateAt(vec3 &targetPos, float perlinHeight);
    
    private:
        //seulement utilisée pour la GÉNÉRATION de terrain
        void addBlock(Block* block);
};

#endif