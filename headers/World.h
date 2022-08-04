#ifndef WORLD_CLASS_H
#define WORLD_CLASS_H

#include<string>
#include<vector>
using namespace std;

#include"Globals.h"
#include"Shape.h"
#include"Quad.h"
#include"Cube.h"
#include"shaderClass.h"
#include"Entity.h"
#include"Camera.h"
#include"GameOverlay.h"
#include"MenuOverlay.h"

class World
{
    //variables statiques
    private:
        vector<Entity*> entities; 
        vector<Shape*> shapes;

        //variables reliées au monde dynamique
        int score;

    public:
        Camera* camera;
        GameOverlay* gameOverlay;
        MenuOverlay* menuOverlay;

        World();

        void render();
        void renderActive3DShapes();
        void renderOverlays();
        void checkCameraCollidingAnyOverlay(glm::vec3 &mousePos);
        vector<int> checkCameraCollidingAnyShape(glm::vec3 &oldPos, glm::vec3 &newPos);
        bool isAnyColliding(vector<int> &collisionLog);
        void addShape(Shape* shape);
        void deleteAllShapes();

        //fonctions reliées à la logique dynamique
        void incrementScore(int amount);
        void updateScore();

    //méthodes pour initialiser le monde
    private:
        void setup3DShapes();

};
#endif