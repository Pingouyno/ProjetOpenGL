#ifndef ENTITY_SNOWMAN_CLASS_H
#define ENTITY_SNOWMAN_CLASS_H

#include"Entity.h"
#include"Cube.h"

class Snowman : public Entity
{
    private:
        Cube* lowerCube;
        Cube* middleCube;
        Cube* upperCube;

    public:
        //l'entité à suivre
        Entity* targetEntity;

        Snowman(glm::vec3 pos, Entity* targetEntity);
        
        //fonctions redéfinies
        function <void(void)> getDefaultClassBehavior();

    private:
        void initSnowman();

};
#endif