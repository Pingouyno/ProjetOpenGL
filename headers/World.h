#ifndef WORLD_CLASS_H
#define WORLD_CLASS_H

#include<string>
#include<vector>
using namespace std;

#include"GlobalArrays.h"
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
    public:
        enum State : int {GAME = 0, MENU = 1};
        World();

    private:
        vector<Entity*> entities; 
        vector<Shape*> shapes;

    public:
        State state;
        Shader* shaderProgram2D;
        Shader* shaderProgram3D;
        Overlay gameOverlay;
        Overlay menuOverlay;
        Camera camera;

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