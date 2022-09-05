#ifndef ITEM_CLASS_H
#define ITEM_CLASS_H

#include"Entity.h"

class EntityItem : public Entity
{
    public:
        const static float ITEM_CUBE_SIZE;
        Cube3D* itemCube;
        EntityItem(vec3 pos, Texture* tex);

        void doAnimation();
        void getAttackedBy(Entity* attacker);
    
        function<void(void)> getDefaultClassBehavior();
};
#endif