#ifndef ENTITY_SNOWMAN_CLASS_H
#define ENTITY_SNOWMAN_CLASS_H

#include"Entity.h"

class Snowman : public Entity
{
    private:
        Quad* faceQuad;

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