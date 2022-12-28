#ifndef ENTITY_PLAYER_CLASS_H
#define ENTITY_PLAYER_CLASS_H

#include"Entity.h"
#include"Camera.h"
#include"Globals.h"

class Player : public Entity
{    
    public:
        static float NORMAL_SPEED;
        static float FAST_SPEED;
        static float FLYING_FAST_SPEED;
        static float FLYING_SURVIVAL_SPEED;

        Camera* camera;
        Texture* textureInHand;

        Player(glm::vec3 pos);

        void getAttackedBy(Entity* attacker);
};
#endif