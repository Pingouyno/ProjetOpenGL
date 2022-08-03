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
#include"Overlay.h"

class World
{
    //variables statiques
    private:
        vector<Entity*> entities = {}; 
        vector<Shape*> shapes = {};

    public:
        Overlay* gameOverlay = nullptr;
        Overlay* menuOverlay = nullptr;
        Camera* camera = nullptr;

        World();

        void render();
        void renderActive3DShapes();
        void renderOverlays();
        void checkCameraCollidingAnyOverlay(glm::vec3 &mousePos);
        vector<int> checkCameraCollidingAnyShape(glm::vec3 &oldPos, glm::vec3 &newPos);
        bool isAnyColliding(vector<int> &collisionLog);
        void addShape(Shape* shape);
        void deleteAllShapes();

    //méthodes pour initialiser le monde
    private:
        void setup3DShapes();

};
#endif