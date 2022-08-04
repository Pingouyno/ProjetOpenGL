#include"../headers/Player.h"

Player::Player(glm::vec3 pos) : Entity(glm::vec3(NAN, NAN, NAN))
{
    this->camera = new Camera(screenWidth, screenHeight, pos);
    //car un joueur ne devrait pas avoir de formes
    this->active = false;
}

glm::vec3& Player::getPos()
{
    return camera->Position;
}

float Player::getPos(int i)
{
    return camera->Position[i];
}

void Player::setPos(glm::vec3 &newPos)
{
    this->camera->Position = newPos;
}