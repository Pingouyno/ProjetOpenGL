#ifndef ENTITY_CLASS_H
#define ENTITY_CLASS_H

#include<functional> 
#include<cmath>
using namespace std;

#include"Shape.h"
#include"Quad.h"
#include"Cube.h"

class Entity
{
    public :  
        /* DIRECTIONS : 

                Z+
             X-    X+
                Z-
        */
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
        Direction dirFacing;
        
    public:
        bool active;
        Entity(glm::vec3 pos);

        void render();
        void doBehavior();
        void setBehavior(function<void(void)> behavior);
        void addShape(Shape* ptrShape);
        void moveTo(glm::vec3 newPos);
        void rotate(vec3 axis, float radians);
        void lookAtHorizontal(vec3 targetPos);
        vec3 getLocalEquivalent(vec3 axis);
        vec3 getXAxis();
        vec3 getYAxis();
        vec3 getZAxis();

        //fonctions à redéfinir (facultatif)
        virtual glm::vec3& getPos();
        virtual float getPos(int i);
        //seulement pour usage interne, voir moveTo() à la place
        virtual void setPos(glm::vec3 &newPos);
        virtual void doAnimation();
        
        //fonctions à redéfinir OBLIGATOIREMENT
        //virtual void setDirFacing(Direction dirFacing);
        virtual function<void(void)> getDefaultClassBehavior();

};
#endif