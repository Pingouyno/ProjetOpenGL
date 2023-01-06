#ifndef WORLD_CLASS_H
#define WORLD_CLASS_H

#include<string>
#include<vector>
using namespace std;
#include <random>

#include"Globals.h"
#include"Shape.h"
#include"Quad.h"
#include"Cube.h"
#include"Cube3D.h"
#include"Block.h"
#include"shaderClass.h"
#include"Entity.h"
#include"Snowman.h"
#include"EntityItem.h"
#include"Camera.h"
#include"Player.h"
#include"GameOverlay.h"
#include"MenuOverlay.h"
#include"GLBufferManager.h"
#include"Chunk.h"
#include"BlockToSpawn.h"

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
        vector<Shape*> shapes;
        vector<Cube3D*> cubes3D;

        vector<vector<Block*>> blocksToRenderMat;
        //x, z (le y n'est pas important)
        vector<vector<Chunk*>> chunkMat;
        vector<Chunk*> loadedChunks;

        //pour les blocs qu'on veut générer mais qui sont dans un chunk non loadé
        vector<BlockToSpawn> blocksToSpawn;

        //variables reliées au monde dynamique
        int score;

        Cube3D* skyBox;
        vector<Quad*> worldBorders;
        vector<Entity*> unloadedEntities; 

    public:
        vector<EntityItem*> entityItems;
        vector<Entity*> loadedEntities;
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

        void addEntity(Entity* entity);
        void addEntityItem(EntityItem* entityItem);
        void killEntity(Entity* entity);
        void killEntityItem(EntityItem* entityItem);

        Entity* getFirstEntityCollidingWithRay(vec3 startingPos, vec3 ray);
        Block* getFirstBlockCollidingWithRay(vec3 startingPos, vec3 ray);
        void checkCameraCollidingAnyOverlay(glm::vec3 &mousePos);
        vec3 checkEntityCollidingAnyShape(Entity* entity);
        vec3 checkEntityCollidingAnyCube(Entity* entity);
        bool isAnyColliding(vec3 &collisionLog);
        void spawnBlockAt(vec3 pos, Texture* tex);
        void despawnBlockAt(vec3 pos);
        void checkEntityTimerDespawns();
        void checkEntityDeaths();
        void checkEntityDeathDespawns();
        Chunk* getChunkAt(vec3 pos);
        Block* getBlockAt(vec3 pos);
        vec3 getPosAdjacentToLookedFace(Block* block, vec3 raySource, vec3 ray);
        void deleteAllShapes();

        //fonctions reliées à la logique dynamique
        void deselectTextBox();
        void setHeldItemSlot(int slot);
        void updateChunks();
        void spawnTreeAt(vec3 pos);

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
        void setupTrees(Chunk* chunk);
        void spawnBlocksAvailableInNewChunk(Chunk* chunk);
};
#endif