#ifndef ENTITY_SNOWMAN_CLASS_H
#define ENTITY_SNOWMAN_CLASS_H

#include"Entity.h"

class Snowman : public Entity
{
    private:
        //formes du cube
        Cube* body;
        Cube* leftArm;
        Cube* rightArm;
        Cube* leftLeg;
        Cube* rightLeg;
        Cube* neck;
        Cube* head;

    public:
        //l'entité à suivre
        Entity* targetEntity;

        Snowman(glm::vec3 pos, Entity* targetEntity);
        
        //fonctions redéfinies
        //void setDirFacing(Direction dirFacing);
        function <void(void)> getDefaultClassBehavior();

    private:
        void initSnowman();

};
#endif