#ifndef ENTITY_SNOWMAN_CLASS_H
#define ENTITY_SNOWMAN_CLASS_H

#include"Entity.h"

class Snowman : public Entity
{
    /* CHASING lorsqu'il marche vers une position, 
       ROAMING lorsqu'il se balade, 
       AGGRO   lorsqu'il pourchasse une entité et veut l'attaquer */
    enum Phase : int {CHASING = 0, ROAMING = 1, AGGRO = 2};
    enum AnimationType : int {WALKING = 0, DYING = 1};

    const float FLYING_MAX_SPEED = DEFAULT_MAX_SPEED / 8;

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
        Phase phase;
        
        //ATTENTION : est parfois nullptr
        Entity* targetEntity = nullptr;
        vec3* targetPos;

        //formes du cube
        Cube3D* body;
        Cube3D* leftArm;
        Cube3D* rightArm;
        Cube3D* leftLeg;
        Cube3D* rightLeg;

    public:
        Cube3D* head;

        //utilisé lorsqu'on met l'entité en mode roaming
        vec3 selfContainedTargetPos;

        Snowman(glm::vec3 pos, vec3* targetPos);
        Snowman(glm::vec3 pos, Entity* targetEntity);
        Snowman(glm::vec3 pos);

        void setAnimation(AnimationType animationType);
        //OBLIGATOIREMENT UTILISER, ne pas modifier directement
        void setTargetEntity(Entity* targetEntity);
        void findNewRandomTargetPos();

        //fonctions redéfinies
        void die();
        void getAttackedBy(Entity* attacker);
        void removeReferenceSingle(Entity* entityToDereference);
        void doAnimation();
        function <void(void)> getDefaultClassBehavior();

    private:
        void doWalkingAnimation();
        void playDeathSound();
        void initSnowman();
};
#endif