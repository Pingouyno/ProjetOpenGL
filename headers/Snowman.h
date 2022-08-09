#ifndef ENTITY_SNOWMAN_CLASS_H
#define ENTITY_SNOWMAN_CLASS_H

#include"Entity.h"

class Snowman : public Entity
{
    private:
        Cube* lowerCube;
        Cube* upperCube;
        Quad* faceQuad;

        Shape* sizeCube;
        Shape* sizeQuad;
        Cube* rotCube;
        Cube* vecCube;

    public:
        //l'entité à suivre
        Entity* targetEntity;

        Snowman(glm::vec3 pos, Entity* targetEntity);
        
        //fonctions redéfinies
        void setDirFacing(Direction dirFacing);
        function <void(void)> getDefaultClassBehavior();

    private:
        void initSnowman();

};
#endif