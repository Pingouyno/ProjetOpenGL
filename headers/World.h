#ifndef WORLD_CLASS_H
#define WORLD_CLASS_H

#include<string>
#include<vector>
using namespace std;

#include"Globals.h"
#include"Shape.h"
#include"Quad.h"
#include"Cube.h"
#include"Cube3D.h"
#include"Block.h"
#include"shaderClass.h"
#include"Entity.h"
#include"Snowman.h"
#include"Camera.h"
#include"Player.h"
#include"GameOverlay.h"
#include"MenuOverlay.h"
#include"GLBufferManager.h"
#include"Chunk.h"

class World
{
    //CONSTANTES
    public:
        //portée du joueur
        const static int PLAYER_RANGE;

        //précision de déplacement du ray lorsqu'on teste la collision
        const static float COLLISION_PRECISION;
        
    //variables statiques
    private:
        vector<Entity*> entities; 
        vector<Shape*> shapes;
        vector<Cube3D*> cubes3D;

        vector<vector<Block*>> blocksToRenderMat;
        //x, z (le y n'est pas important)
        vector<vector<Chunk*>> chunkMat;
        vector<Chunk*> chunksToUnload;

        //variables reliées au monde dynamique
        int score;

        Cube3D* skyBox;

    public:
        Player* player;
        Camera* camera;
        GameOverlay* gameOverlay;
        MenuOverlay* menuOverlay;

        World();

        void doEntityBehaviors();
        void render();
        void renderActiveShapes();
        void renderActiveEntities();
        void renderActive3DCubes();
        void renderActive3DCubesEntities();
        void renderBlocks();
        void renderOverlays();

        Block* getFirstBlockCollidingWithRay(vec3 startingPos, vec3 ray);
        void checkCameraCollidingAnyOverlay(glm::vec3 &mousePos);
        vector<int> checkCameraCollidingAnyShape(glm::vec3 &oldPos, glm::vec3 &newPos);
        bool isAnyColliding(vector<int> &collisionLog);
        void spawnBlockAt(vec3 pos, Texture* tex);
        void despawnBlockAt(vec3 pos);
        Chunk* getChunkAt(vec3 pos);
        Block* getBlockAt(vec3 pos);
        vec3 getPosAdjacentToLookedFace(Block* block, vec3 raySource, vec3 ray);
        void deleteAllShapes();

        //fonctions reliées à la logique dynamique
        void incrementScore(int amount);
        void updateScore();
        void deselectTextBox();
        void setHeldItemSlot(int slot);
        void updateChunks();

    //méthodes pour initialiser le monde
    private:
        void addShape(Shape* shape);
        void addCube3D(Cube3D* cube);
        bool isBlockNearAir(Block* block);
        void updateBlock(Block* block);
        void addBlockToRendering(Block* block);
        void removeBlockFromRendering(Block* blockToRemove);
        void setupBlocksToRender(Chunk* chunk);
        void setupEntities();
        void setup3DShapes();
        void setupChunks();
        void loadChunk(Chunk* chunk);
        void unloadChunk(Chunk* chunk);

};
#endif