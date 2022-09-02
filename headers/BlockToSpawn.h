#ifndef STRUCTURE_TO_SPAWN_CLASS_H
#define STRUCTURE_TO_SPAWN_CLASS_H

#include"Texture.h"
#include"../libraries/include/glm/gtc/matrix_transform.hpp"

struct BlockToSpawn
{
    public:
        glm::vec3 positionToSpawnAt;
        Texture* textureToSpawn;
};

#endif