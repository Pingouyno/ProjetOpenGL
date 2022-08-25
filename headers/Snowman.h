#ifndef ENTITY_SNOWMAN_CLASS_H
#define ENTITY_SNOWMAN_CLASS_H

#include"Entity.h"

class Snowman : public Entity
{
    enum AnimationType : int;

    private:
        const static float size;
        const static vec3 headFormat;
        const static vec3 bodyFormat;
        const static vec3 armFormat;
        const static vec3 legFormat;

        //pour avoir une référence pour les animations
        int time;
        int articulationTimer;
        int articulationDirection;
        AnimationType animationType;

        //formes du cube
        Cube3D* body;
        Cube3D* leftArm;
        Cube3D* rightArm;
        Cube3D* leftLeg;
        Cube3D* rightLeg;
        Cube3D* head;

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