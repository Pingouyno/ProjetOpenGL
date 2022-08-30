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
#include"PerlinNoise.h"
#include"GLBufferManager.h"

class World
{
    //CONSTANTES
    public:
        //portée du joueur
        const static int PLAYER_RANGE;
        //précision de déplacement du ray lorsqu'on teste la collision
        const static float COLLISION_PRECISION;

        const static int WORLD_SIZE;
        const static int WORLD_HEIGHT;
        const static int CHUNK_SIZE;

    //variables statiques
    private:
        //générateur de perlin noise
        PerlinNoise* perlinNoise;
        vector<Entity*> entities; 
        vector<Shape*> shapes;
        vector<Cube3D*> cubes3D;

        //x, y, z
        vector<vector<vector<Block*>>>blockMat;
        vector<vector<Block*>> blocksToRenderMat;

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
        Block* getBlockAt(vec3 pos);
        vec3 getPosAdjacentToLookedFace(Block* block, vec3 raySource, vec3 ray);
        void deleteAllShapes();

        //fonctions reliées à la logique dynamique
        void incrementScore(int amount);
        void updateScore();
        void deselectTextBox();
        void setHeldItemSlot(int slot);

    //méthodes pour initialiser le monde
    private:
        void addShape(Shape* shape);
        void addCube3D(Cube3D* cube);
        void addBlock(Block* block);
        bool isBlockNearAir(Block* block);
        void updateBlock(Block* block);
        void addBlockToRendering(Block* block);
        void removeBlockFromRendering(Block* blockToRemove);
        void setupBlocksToRender();
        void setupEntities();
        void setup3DShapes();

};
#endif