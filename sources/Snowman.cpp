#include"../headers/Snowman.h"

enum Snowman::AnimationType : int {WALKING = 0};

Snowman::Snowman(glm::vec3 pos, Entity* targetEntity) : Entity(pos)
{
    setAnimation(WALKING);
    this->targetEntity = targetEntity;
    this->behavior = getDefaultClassBehavior();
    initSnowman();
}

/*
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
*/

void Snowman::setAnimation(AnimationType animationType)
{
    this->animationType = animationType;
    this->time = 0;

    this->articulationDirection = 1;
    this->articulationTimer = 0;
}

void Snowman::doAnimation()
{
    switch(animationType)
    {
        case WALKING:
            doWalkingAnimation();
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

        //mouvements de l'entité
        moveTo(getPos() + glm::vec3(speed * factX, speed * factY, speed * factZ));
        this->lookAtHorizontal(targetEntity->getPos());
        head->lookAt(targetEntity->getPos() + vec3(0, 1, 0));
        doAnimation();
        time++;
    };  
}

void Snowman::doWalkingAnimation()
{
    int animationLength = 30;
    float rotation = (RADIAN_CIRCLE / 4) / animationLength;

    //animation des articulations
    this->rightArm->rotateAround(getPos() + getLocalEquivalent(vec3(0, 1.5, 0)), getXAxis(), rotation * articulationDirection);
    this->leftArm->rotateAround(getPos() + getLocalEquivalent(vec3(0, 1.5, 0)), getXAxis(), rotation * -articulationDirection);
    this->rightLeg->rotateAround(getPos() + getLocalEquivalent(vec3(0, -2.5, 0)), getXAxis(), rotation * -articulationDirection);
    this->leftLeg->rotateAround(getPos() + getLocalEquivalent(vec3(0, -2.5, 0)), getXAxis(), rotation * articulationDirection);

    //incrémenter et déterminer la prochaine direction
    articulationTimer += articulationDirection;
    if (articulationTimer == -animationLength / 2 || articulationTimer == animationLength / 2)
    {
        articulationDirection = -articulationDirection;
    }
}

void Snowman::initSnowman()
{
    Texture* wood_png = Texture::get3DImgTexture("wood.png");
    Texture* obama_png = Texture::get3DImgTexture("obama.png");
    
    //corps
    body = new Cube(getPos(), vec3(3, 4, 1.5), wood_png);
    leftArm = new Cube(getPos() + vec3(2, 0.5, 0), vec3(1, 3, 1), wood_png);
    rightArm = new Cube(getPos() + vec3(-2, 0.5, 0), vec3(1, 3, 1), wood_png);
    leftLeg = new Cube(getPos() + vec3(0.75, -3.75, 0), vec3(1.25, 3.5, 1.25), wood_png);
    rightLeg = new Cube(getPos() + vec3(-0.75, -3.75, 0), vec3(1.25, 3.5, 1.25), wood_png);
    neck = new Cube(getPos() + vec3(0, 2.5, 0), vec3(0.75, 1, 0.75), wood_png);
    head = new Cube(getPos() + vec3(0, 3, 0), vec3(1.5, 1.5, 1.5), wood_png);

    entityShapes = 
    {
        body,
        leftArm,
        rightArm,
        leftLeg,
        rightLeg,
        neck,
        head
    };
}