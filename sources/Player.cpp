#include"../headers/Player.h"

float Player::NORMAL_SPEED = 0.10f;
float Player::FAST_SPEED = 2 * NORMAL_SPEED;
float Player::FLYING_FAST_SPEED = 4 * NORMAL_SPEED;
float Player::FLYING_SURVIVAL_SPEED = NORMAL_SPEED / 2;

Player::Player(glm::vec3 pos) : Entity(pos)
{
    this->camera = new Camera(screenWidth, screenHeight, &(this->pos));
    //car un joueur ne devrait pas avoir de formes
    this->active = false;
    this->textureInHand = nullptr;
    this->maxSpeed = NORMAL_SPEED;

    this->hitBox = new Cube3D(getPos(), vec3(Shape::camBoxWidth, Shape::camBoxHeight, Shape::camBoxWidth), Texture::Air);
    this->hitBox->active = false;
    this->hitBoxDimensions = vec3(Shape::camBoxWidth, Shape::camBoxHeight, Shape::camBoxWidth);
    addCube3D(hitBox);
}

void Player::getAttackedBy(Entity* attacker)
{
    PlaySound::playPlayerHitSound();
}