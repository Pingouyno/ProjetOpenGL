#ifndef ITEM_CLASS_H
#define ITEM_CLASS_H

#include"Entity.h"

class EntityItem : public Entity
{
    public:
        const static float ITEM_CUBE_SIZE;
        const static int DESPAWN_TIMER_FRAMES;
        const static int PICKUP_TIMER_FRAME;

        int framesUntilDespawn;
        Cube3D* itemCube;
        EntityItem(vec3 pos, Texture* tex);

        void die();
        int getAttackImmuneFrameConst();
        void getAttackedBy(Entity* attacker);

        void doAnimation();
        void playDeathSound();
        function<void(void)> getDefaultClassBehavior();
};
#endif