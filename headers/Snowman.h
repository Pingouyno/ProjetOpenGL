#ifndef ENTITY_SNOWMAN_CLASS_H
#define ENTITY_SNOWMAN_CLASS_H

#include"Entity.h"

class Snowman : public Entity
{
    enum AnimationType : int;

    private:
        //pour avoir une référence pour les animations
        int time;
        int articulationTimer;
        int articulationDirection;
        AnimationType animationType;

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

        void setAnimation(AnimationType animationType);
        void doAnimation();
        
        //fonctions redéfinies
        //void setDirFacing(Direction dirFacing);
        function <void(void)> getDefaultClassBehavior();

    private:
        void doWalkingAnimation();
        void initSnowman();
};
#endif