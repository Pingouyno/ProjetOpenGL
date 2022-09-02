#include"../headers/Snowman.h"

const float Snowman::size = 0.2f;
const vec3 Snowman::headFormat = vec3(size*1.5, size*1.5, size*1.5);
const vec3 Snowman::bodyFormat = vec3(size*2.5, size*3.25, size*1.5);
const vec3 Snowman::armFormat = vec3(size*0.9f, size*3, size*0.9f);
const vec3 Snowman::legFormat = vec3(size*1.25, size*3.5, size*1.25);

enum Snowman::AnimationType : int {WALKING = 0};

Snowman::Snowman(glm::vec3 pos, Entity* targetEntity) : Entity(pos)
{
    setAnimation(WALKING);
    this->targetEntity = targetEntity;
    this->behavior = getDefaultClassBehavior();
    initSnowman();
}

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
        float distZ = this->getPos(2) - targetEntity->getPos(2);

        //trouver le sens du mouvement
        float factX = distX < 0 ? 1 : -1;
        float factZ = distZ < 0 ? 1 : -1;

        //arrêter de trembler si l'axe est proche
        if (abs(distX) < minDistX) factX = 0;
        if (abs(distZ) < minDistX) factZ = 0;

        //mouvements de l'entité
        setVelocity(vec3(speed * factX, velocity.y - JUMP_FALL_ACCELERATION, speed * factZ));
        this->lookAtHorizontal(targetEntity->getPos());
        head->lookAt(targetEntity->getPos());
        doAnimation();
        time++;
    };  
}

void Snowman::doWalkingAnimation()
{
    int animationLength = 30;
    float rotation = (RADIAN_CIRCLE / 4) / animationLength;

    //animation des articulations
    this->rightArm->rotateAround(getPos() + getLocalEquivalent(vec3(0, bodyFormat.y/2 - armFormat.y/4, 0)), getXAxis(), rotation * articulationDirection);
    this->leftArm->rotateAround(getPos() + getLocalEquivalent(vec3(0, bodyFormat.y/2 - armFormat.y/4, 0)), getXAxis(), rotation * -articulationDirection);
    this->rightLeg->rotateAround(getPos() + getLocalEquivalent(vec3(0, -bodyFormat.y/2, 0)), getXAxis(), rotation * -articulationDirection);
    this->leftLeg->rotateAround(getPos() + getLocalEquivalent(vec3(0, -bodyFormat.y/2, 0)), getXAxis(), rotation * articulationDirection);

    //incrémenter et déterminer la prochaine direction
    articulationTimer += articulationDirection;
    if (articulationTimer == -animationLength / 2 || articulationTimer == animationLength / 2)
    {
        articulationDirection = -articulationDirection;
    }
}

void Snowman::initSnowman()
{
    Texture* wood_png = Texture::get2DImgTexture("wood.png");
    Texture* obama_png = Texture::get2DImgTexture("obama.png");
    
    //corps
    body = new Cube3D(getPos(), bodyFormat, Texture::get3DImgTexture(Texture::TEX3D::STEVE_BODY));
    leftArm = new Cube3D(getPos() + vec3((bodyFormat.x + armFormat.x)/2, (bodyFormat.y - armFormat.y)/2, 0), armFormat, Texture::get3DImgTexture(Texture::TEX3D::STEVE_LEFT_ARM));
    rightArm = new Cube3D(getPos() + vec3(-(bodyFormat.x + armFormat.x)/2, (bodyFormat.y - armFormat.y)/2, 0), armFormat, Texture::get3DImgTexture(Texture::TEX3D::STEVE_RIGHT_ARM));
    leftLeg = new Cube3D(getPos() + vec3((bodyFormat.x - legFormat.x)/2, -(bodyFormat.y + legFormat.y)/2 + legFormat.y/12, 0), legFormat, Texture::get3DImgTexture(Texture::TEX3D::STEVE_LEFT_LEG));
    rightLeg = new Cube3D(getPos() + vec3(-(bodyFormat.x - legFormat.x)/2, -(bodyFormat.y + legFormat.y)/2 + legFormat.y/12, 0), legFormat, Texture::get3DImgTexture(Texture::TEX3D::STEVE_RIGHT_LEG));
    head = new Cube3D(getPos() + vec3(0, (bodyFormat.y + headFormat.y)/2, 0), headFormat, Texture::get3DImgTexture(Texture::TEX3D::STEVE_HEAD));

    entityCubes3D = 
    {
        body,
        leftArm,
        rightArm,
        leftLeg,
        rightLeg,
        head
    };

    //hitbox
    const float width = body->width + 2*leftLeg->width;
    const float height = body->height + head->height + leftLeg->height;
    const float depth = body->depth;
    vec3 hitBoxCenter = getPos() + vec3(0, body->height/2 + head->height - height/2, 0);
    hitBox = new Cube3D(hitBoxCenter, vec3((width + depth)/2, height, (width + depth)/2), Texture::Air);
}