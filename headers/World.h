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
#include"shaderClass.h"
#include"Entity.h"
#include"Snowman.h"
#include"Camera.h"
#include"Player.h"
#include"GameOverlay.h"
#include"MenuOverlay.h"
#include"PerlinNoise.h"

class World
{
    //variables statiques
    private:
        //générateur de perlin noise
        PerlinNoise* perlinNoise;
        vector<Entity*> entities; 
        vector<Shape*> shapes;
        vector<Cube3D*> cubes3D;

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
        void renderOverlays();
        Cube3D* getFirstCubeCollidingWithRay(vec3 startingPos, vec3 ray);
        void checkCameraCollidingAnyOverlay(glm::vec3 &mousePos);
        vector<int> checkCameraCollidingAnyShape(glm::vec3 &oldPos, glm::vec3 &newPos);
        bool isAnyColliding(vector<int> &collisionLog);
        void addShape(Shape* shape);
        void addCube3D(Cube3D* cube);
        void deleteAllShapes();

        //fonctions reliées à la logique dynamique
        void incrementScore(int amount);
        void updateScore();
        void deselectTextBox();

    //méthodes pour initialiser le monde
    private:
        void setupEntities();
        void setup3DShapes();

};
#endif