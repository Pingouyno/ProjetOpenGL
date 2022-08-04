#ifndef ENTITY_PLAYER_CLASS_H
#define ENTITY_PLAYER_CLASS_H

#include"Entity.h"
#include"Camera.h"
#include"Globals.h"

class Player : public Entity
{    
    public:
        Camera* camera;
        Player(glm::vec3 pos);
        
        glm::vec3& getPos();
        float getPos(int i);
        void setPos(glm::vec3 &newPos);
};
#endif