#include"../headers/Snowman.h"

Snowman::Snowman(glm::vec3 pos, Entity* targetEntity) : Entity(pos)
{
    this->targetEntity = targetEntity;
    this->behavior = getDefaultClassBehavior();
    initSnowman();
}

//traquer la cible
function <void(void)> Snowman::getDefaultClassBehavior()
{
    //1 bloc/seconde
    float speed = 1.0f/60.0f;
    return [this, speed]()
    {
        //trouver le sens du mouvement
        float factX = this->getPos(0) - targetEntity->getPos(0) < 0 ? 1 : -1;
        float factY = this->getPos(1) - targetEntity->getPos(1) < 0 ? 1 : -1;
        float factZ = this->getPos(2) - targetEntity->getPos(2) < 0 ? 1 : -1;

        moveTo(getPos() + glm::vec3(speed * factX, speed * factY, speed * factZ));
    };
}

void Snowman::initSnowman()
{
    float cubeSize = 1.0f;
    glm::vec3 oneSize = glm::vec3(0, cubeSize, 0);
    glm::vec3 color(180, 180, 180);

    Cube* lowerCube = new Cube(getPos(), cubeSize, color);
    Cube* middleCube = new Cube(getPos() + oneSize, cubeSize, color);
    Cube* upperCube = new Cube(getPos() + oneSize + oneSize, cubeSize, color);

    entityShapes = 
    {
        lowerCube,
        middleCube,
        upperCube
    };
}