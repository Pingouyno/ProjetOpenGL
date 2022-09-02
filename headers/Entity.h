#ifndef ENTITY_CLASS_H
#define ENTITY_CLASS_H

#include<functional> 
#include<cmath>
using namespace std;

#include"Shape.h"
#include"Quad.h"
#include"Cube.h"
#include"Cube3D.h"

class Entity
{
    public :  
        enum Direction: char {NORTH = 'N', WEST = 'W', EAST = 'E', SOUTH = 'S'};
        const static Direction DEFAULT_DIRECTION = NORTH;
        const static float RADIAN_CIRCLE;

    private:
        //privé car on doit le protéger pour Player (qui utilise camera.pos)
        glm::vec3 pos;
        mat4 originTransposition;

    protected:
        function<void(void)> behavior;
        vector<Shape*> entityShapes;
        vector<Cube3D*> entityCubes3D;
        vector<Entity*> subEntities;
        Cube3D* hitBox;
        Direction dirFacing;
        
    public:
        vec3 velocity;
        bool active;
        Entity(glm::vec3 pos);

        void render();
        void render3DCubes();
        void doBehavior();
        void setBehavior(function<void(void)> behavior);
        void addShape(Shape* ptrShape);
        void addCube3D(Cube3D* ptrCube);
        void addEntity(Entity* entity);
        void moveTo(glm::vec3 newPos);
        void setVelocity(vec3 velocityToAdd);
        void addVelocity(vec3 velocityToAdd);
        void resetVelocity();
        void moveToVelocity();
        vec3 getPotentialNewPos();
        void jump();
        void rotate(vec3 axis, float radians);
        void rotateAround(vec3 pos, vec3 axis, float radians);
        void lookAtHorizontal(vec3 targetPos);
        bool isColliding(vec3 pos);
        bool wouldThenBeCollidingCube(vec3 &testedVelocity, Cube3D* worldCube);
        void reportCollisionWithCubeThen(vec3 &collisionLog, Cube3D* worldCube);
        vec3 getLocalEquivalent(vec3 axis);
        vec3 getXAxis();
        vec3 getYAxis();
        vec3 getZAxis();

        //fonctions à redéfinir (facultatif)
        virtual glm::vec3& getPos();
        virtual float getPos(int i);
        virtual void doAnimation();
        
        //fonctions à redéfinir OBLIGATOIREMENT
        //virtual void setDirFacing(Direction dirFacing);
        virtual function<void(void)> getDefaultClassBehavior();

    protected:
        //seulement pour usage interne, voir moveTo() à la place
        virtual void setPos(glm::vec3 newPos);

};
#endif