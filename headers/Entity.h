#ifndef ENTITY_CLASS_H
#define ENTITY_CLASS_H

#include<functional> 
using namespace std;

#include"Shape.h"

class Entity
{
    private:
        //privé car on doit le protéger pour Player (qui utilise camera.pos)
        glm::vec3 pos;
    protected:
        function<void(void)> behavior;
        vector<Shape*> entityShapes;

    public:
        bool active;
        Entity(glm::vec3 pos);

        void render();
        void doBehavior();
        void setBehavior(function<void(void)> behavior);
        void addShape(Shape* ptrShape);
        
        void moveTo(glm::vec3 newPos);

        virtual glm::vec3& getPos();
        virtual float getPos(int i);
        //seulement pour usage interne, voir moveTo() à la place
        virtual void setPos(glm::vec3 &newPos);
        
        //fonctions à redéfinir
        virtual function<void(void)> getDefaultClassBehavior();

};
#endif