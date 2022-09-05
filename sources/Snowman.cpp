#include"../headers/Snowman.h"

const float Snowman::size = 0.2f;
const vec3 Snowman::headFormat = vec3(size*1.6, size*1.6, size*1.6);
const vec3 Snowman::bodyFormat = vec3(size*2.5, size*3.25, size*1.5);
const vec3 Snowman::armFormat = vec3(size*0.9f, size*3, size*0.9f);
const vec3 Snowman::legFormat = vec3(size*1.25, size*3.5, size*1.25);

Snowman::Snowman(glm::vec3 pos, vec3* targetPos) : Entity(pos)
{
    this->phase = CHASING;
    selfContainedTargetPos = vec3(NAN);
    setAnimation(WALKING);
    this->targetPos = targetPos;
    this->targetEntity = nullptr;
    this->behavior = getDefaultClassBehavior();
    initSnowman();
}

Snowman::Snowman(vec3 pos, Entity* targetEntity) : Snowman(pos, &targetEntity->getPos())
{
    this->phase = AGGRO;
    setTargetEntity(targetEntity);
}

Snowman::Snowman(glm::vec3 pos) : Snowman(pos, &selfContainedTargetPos)
{
    this->phase = ROAMING;
    findNewRandomTargetPos();
}

void Snowman::setAnimation(AnimationType animationType)
{
    this->animationType = animationType;
    this->time = 0;

    this->articulationDirection = 1;
    this->articulationTimer = 0;
}

void Snowman::setTargetEntity(Entity* targetEntity)
{
    if (targetEntity != nullptr)
    {
        removeReference(targetEntity);
    }
    this->targetEntity = targetEntity;
    this->targetPos = &targetEntity->getPos();
    addReference(targetEntity);
}

void Snowman::findNewRandomTargetPos()
{
    const vec3 selfPos = getPos();

    const float range = 16;
    std::random_device dev;
   	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> distFindCoord(0, range);
    
    const float newX = selfPos.x + distFindCoord(rng) - range/2;
    const float newZ = selfPos.z + distFindCoord(rng) - range/2;
    const float newY = Chunk::getPerlinHeightOf(newX, newZ) + 2;

    selfContainedTargetPos = vec3(newX, newY, newZ);
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

void Snowman::getAttackedBy(Entity* attacker)
{
    this->phase = AGGRO;
    setTargetEntity(attacker);
}

//traquer la cible
function <void(void)> Snowman::getDefaultClassBehavior()
{
    float minDistX = 0.7f;
    float minDistY = 0.1f;
    
    int* num = new int[1]{-1};
    int* cpt = &num[0];

    return [this, minDistX, minDistY, cpt]()
    {
        //trouver les distances
        float distX = this->getPos(0) - targetPos->x;
        float distY = this->getPos(1) - targetPos->y;
        float distZ = this->getPos(2) - targetPos->z;

        //trouver le sens du mouvement
        float factX = distX < 0 ? 1 : -1;
        float factZ = distZ < 0 ? 1 : -1;

        //arrêter de trembler si l'axe est proche
        if (abs(distX) < minDistX) factX = 0;
        if (abs(distZ) < minDistX) factZ = 0;
        
        const bool wasTouchingGround = isTouchingGround();

        /*si velocity Y était en collision et X ou Z, alors sauter seulement si on est en train de tomber (velY < 0))
        on le fait après car on ne sait pas si le saut causera une collision*/
        if (collisionLog.y != 0 && previousRawVelocity.y < 0 && (collisionLog.x != 0 || collisionLog.z != 0))
        {
            jump();
        }

        //appliquer la gravité, important de mettre ça APRÈS le jump
		addVelocity(Entity::FALLING_VELOCITY);

        /*mouvements de l'entité, si velocité.y != 0 alors on est dans les airs et en train de tomber.
         par conséquent on perd notre grippe sur le sol (add plutot que set velocity)
         NOTE : ne pas faire de addVelocityCapped() dans le vide, sinon les steves s,attaquent plusieurs 
                fois en une seconde car ils marchent l'un vers l'autre*/ 
        if (wasTouchingGround)
        {
            this->maxSpeed = Entity::DEFAULT_MAX_SPEED;
            setVelocity(vec3(DEFAULT_MAX_SPEED * factX, velocity.y, DEFAULT_MAX_SPEED * factZ));
        }else
        {
            this->maxSpeed = FLYING_MAX_SPEED;
            addVelocityCapped(vec3(DEFAULT_MAX_SPEED/4 * factX, 0, FLYING_MAX_SPEED * factZ));
        }

        //un dégradé de la rotation pour être plus naturel
        vec3 posLookedByBody = getPos() + getZAxis() + ((glm::normalize(velocity) - getZAxis()) / 2.0f);
        vec3 posLookedByHead = *targetPos;
        //faire que la tête regarde la tête de l"autre entité
        if ((phase == CHASING || phase == AGGRO) && targetEntity != nullptr) posLookedByHead += vec3(0, targetEntity->hitBox->height / 16.0f, 0);

        if (wasTouchingGround) lookAtHorizontal(posLookedByBody);
        head->lookAt(posLookedByHead);
        doAnimation();
        time++;

        /*trouver une nouvelle destination si on a touché la destination cible.
         recommencer à bouger seulement après un certain décompte*/
        if (this->phase == ROAMING && abs(distX) < minDistX && abs(distZ) < minDistX)
        {
            if (*cpt == -1)
            {   
                const int maxFramesToWait = 60 * 5;
                std::random_device dev;
                std::mt19937 rng(dev());
                std::uniform_int_distribution<std::mt19937::result_type> distTimeToWait(0, maxFramesToWait);
                *cpt = distTimeToWait(rng);
            }else if (*cpt == 0)
            {
                findNewRandomTargetPos();
                *cpt = -1;
            }
            else *cpt -= 1;
        }

        //attaquer l'autre entité si on est en mode pourchassement
        if (this->phase == AGGRO && wouldBeCollidingEntityVelocity(targetEntity))
        {
            //probabiilité d'attaquer car on ne veut pas que ce soit toujours le même qui attaque
	        const int ATTACK_PROBABILITY = 2;

            std::random_device dev;
            std::mt19937 rng(dev());
            std::uniform_int_distribution<std::mt19937::result_type> distAttack(1, ATTACK_PROBABILITY); 
            if (distAttack(rng) == 1) 
            {
                attackEntity(targetEntity);
            }
        }
    };  
}

void Snowman::doWalkingAnimation()
{
    //si la vélocité est nulle alors on peut figer les bras à la position neutre
    if (!(articulationTimer == 0 && !(velocity.x || velocity.z)))
    {
        const int animationLength = 30;
        const float rotation = (RADIAN_CIRCLE / 4) / animationLength;

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
    hitBox->active = false;
    this->hitBoxDimensions = vec3(hitBox->width, hitBox->height, hitBox->depth);
    addCube3D(hitBox);
}