#include"../headers/Snowman.h"

Snowman::Snowman(glm::vec3 pos, Entity* targetEntity) : Entity(pos)
{
    this->targetEntity = targetEntity;
    this->behavior = getDefaultClassBehavior();
    initSnowman();
}

//On rappelle que Z sud est négatif (sud)
void Snowman::setDirFacing(Direction dirFacing)
{
    this->dirFacing = dirFacing;
    switch(dirFacing)
    {
        case NORTH:
            faceQuad->setAxis(Quad::Axis::Z);
            faceQuad->moveTo(getPos() + glm::vec3(0, 0, faceQuad->width + 0.02f));
            break;
        
        case WEST:
            faceQuad->setAxis(Quad::Axis::X);
            faceQuad->moveTo(getPos() + glm::vec3(faceQuad->width + 0.02f, 0, 0.0f));
            break;
        
        case EAST:
            faceQuad->setAxis(Quad::Axis::X);
            faceQuad->moveTo(faceQuad->pos + glm::vec3(-0.02f, 0, 0));
            break;

        case SOUTH:
            faceQuad->setAxis(Quad::Axis::Z);
            faceQuad->moveTo(faceQuad->pos + glm::vec3(0, 0, -0.02f));
            break;
    };
}

//traquer la cible
function <void(void)> Snowman::getDefaultClassBehavior()
{
    //2 bloc/seconde
    float speed = 2.0f/60.0f;
    float minDistX = 0.7f;
    float minDistY = 0.1f;
    
    int* num = new int[1]{0};
    int* cpt = &num[0];

    return [this, speed, minDistX, minDistY, cpt]()
    {
        //trouver les distances
        float distX = this->getPos(0) - targetEntity->getPos(0);
        float distY = this->getPos(1) - targetEntity->getPos(1);
        float distZ = this->getPos(2) - targetEntity->getPos(2);

        /*trouver le sens de l'orientation du visage
        if (abs(distX) > abs(distZ))
        {
            distX > 0 ? setDirFacing(EAST) : setDirFacing(WEST);
        }else
        {
            distZ > 0 ? setDirFacing(SOUTH) : setDirFacing(NORTH);
        }
        */

        //trouver le sens du mouvement
        float factX = distX < 0 ? 1 : -1;
        float factY = distY < 0 ? 1 : -1;
        float factZ = distZ < 0 ? 1 : -1;

        //arrêter de trembler si l'axe est proche
        if (abs(distX) < minDistX) factX = 0;
        if (abs(distY) < minDistY) factY = 0;
        if (abs(distZ) < minDistX) factZ = 0;

        moveTo(getPos() + glm::vec3(speed * factX, speed * factY, speed * factZ));

        faceQuad->lookAt(targetEntity->getPos());
    };
}

void Snowman::initSnowman()
{
    //corps
    vec3 colorBlack(0, 0, 0);
    float cubeSize = 2.0f; 
    float lowWidth = 0.2f;
    float lowHeight = 1.5f;

    //visage
    faceQuad = new Quad(getPos(), cubeSize, Texture::get3DImgTexture("obama.png"), Quad::Axis::Z);

    entityShapes = 
    {
        faceQuad
    };
}