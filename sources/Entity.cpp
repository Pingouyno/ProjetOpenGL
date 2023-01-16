#include"../headers/Entity.h"

const float Entity::RADIAN_CIRCLE = 2*M_PI;
//force G qui nous donne x * 1u/s d'accélération
const float Entity::JUMP_FALL_ACCELERATION = 9.8 * 1.0f / 1830.0f;
const vec3 Entity::JUMPING_VELOCITY = vec3(0, 0.12f, 0);
const vec3 Entity::FALLING_VELOCITY = vec3(0, -JUMP_FALL_ACCELERATION, 0);
//utilisée dans le mode survie
const float Entity::DEFAULT_MAX_SPEED = 2.0f/60.0f; //2 blocs par seconde

const int Entity::ATTACK_COOLDOWN_FRAME = 60.0f/2.0f; //une demie de seconde, car en frames

const int Entity::DEFAULT_HEALTH = 3;

const int Entity::DEATH_TIMER_FRAME = 45;

Entity::Entity(glm::vec3 pos)
{
    this->pos = pos;
    this->behavior = [](){};
    this->entityShapes = {};
    this->entityCubes3D = {};
    this->subEntities = {};
    this->active = true;
    this->originTransposition = mat4(1.0f);
    this->velocity = vec3(0);
    this->previousRawVelocity = vec3(0);
    this->hitBox = nullptr;
    this->hitBoxDimensions = vec3(NAN);
    this->referencedEntities = {};
    this->maxSpeed = DEFAULT_MAX_SPEED;
    this->collisionLog = vec3(NAN);
    this->canGetPlacedBlockOn = false;
    this->health = getDefaultHealth();
    //utilisé pour les classes anonymes
    initEntity();
}

void Entity::render()
{
    for (Shape* ptrShape : entityShapes)
    {
        if (ptrShape->active)
        {
            ptrShape->render();
        }
    }
    for (Cube3D* ptrCube : entityCubes3D)
    {
        if (ptrCube->active)
        {
            ptrCube->render();
        }
    }
    for (Entity* ptrEntity : subEntities)
    {
        if (ptrEntity->active)
        {
            ptrEntity->render();
        }
    }
    //hitBox->render();
}

void Entity::render3DCubes()
{
    for (Cube3D* ptrCube : entityCubes3D)
    {
        if (ptrCube->active)
        {
            ptrCube->render();
        }
    }
    for (Entity* ptrEntity : subEntities)
    {
        if (ptrEntity->active)
        {
            ptrEntity->render3DCubes();
        }
    }
}

//à la fin de l'immunité on arrête d'être rouge
void Entity::checkEndOfAttackImmuneTimer()
{
    this->framesUntilattackImmuneEnd--;
    //reset le timer si on a franchi la date
    if (this->isAttackImmune && this->framesUntilattackImmuneEnd == 0)
    {
        this->isAttackImmune = false;
        setRed(false);
    }
}

void Entity::doBehavior()
{
	checkEndOfAttackImmuneTimer();
    this->behavior();
}

void Entity::setRed(bool isRed)
{
    for (Entity* subEntity : this->subEntities)
    {
        subEntity->setRed(isRed);
    }

    if (isRed)
    {
        for (Cube3D* cube : this->entityCubes3D)
        {
            cube->setToRed(255.0f);
        }
    }
    else
    {
        for (Cube3D* cube : this->entityCubes3D)
        {
            cube->setToBrightness(1.0f);
        }
    }
}

void Entity::setBehavior(function<void(void)> behavior)
{
    this->behavior = behavior;
}

//utiliser addShape3D pour les cubes à texture 3D
void Entity::addShape(Shape* ptrShape)
{
    entityShapes.push_back(ptrShape);
}

void Entity::addCube3D(Cube3D* ptrCube)
{
    entityCubes3D.push_back(ptrCube);
}

void Entity::addEntity(Entity* entity)
{
    this->subEntities.push_back(entity);
}

void Entity::moveTo(glm::vec3 newPos)
{
    for (Shape* ptrShape : entityShapes)
    {
        //déplacer les éléments de l'équivalent de la différence (x, y, z) entre newPos et oldPos
        ptrShape->moveTo(ptrShape->pos + (newPos - getPos()));
    }
    for (Cube3D* ptrCube : entityCubes3D)
    {
        ptrCube->moveTo(ptrCube->pos + (newPos - getPos()));
    }
    for (Entity* ptrEntity : subEntities)
    {
        ptrEntity->moveTo(ptrEntity->getPos() + (newPos - getPos()));
    }
    this->pos = newPos;
}

void Entity::setVelocity(vec3 velocity)
{
    this->velocity = velocity;
}

void Entity::addVelocity(vec3 velocityToAdd)
{
    this->velocity += velocityToAdd;
}

//ajoute avec une limite, selon la maxSpeed actuelle. NOTE : aucune limmite pour le y
void Entity::addVelocityCapped(vec3 velocityToAdd)
{
    this->velocity.y += velocityToAdd.y;

    float potentialVelX = this->velocity.x + velocityToAdd.x;
    if (abs(this->velocity.x) < maxSpeed || abs(potentialVelX) < abs(this->velocity.x))
    {
        if (abs(potentialVelX > maxSpeed))
        {
            const float signX = potentialVelX < 0 ? -1 : 1;
            potentialVelX = signX * maxSpeed;
        }
        this->velocity.x = potentialVelX;
    }

    float potentialVelZ = this->velocity.z + velocityToAdd.z;
    if (abs(this->velocity.z) < maxSpeed || abs(potentialVelZ) < abs(this->velocity.z))
    {
        if (abs(potentialVelZ > maxSpeed))
        {
            const float signZ = potentialVelZ < 0 ? -1 : 1;
            potentialVelZ = signZ * maxSpeed;
        }
        this->velocity.z = potentialVelZ;
    }
}


void Entity::resetVelocity()
{
    velocity = vec3(0, 0, 0);
    for (Entity* ptrEntity : subEntities)
    {
        ptrEntity->resetVelocity();
    }
}

void Entity::moveToVelocity()
{
    moveTo(getPotentialNewPos());
}

vec3 Entity::getPotentialNewPos()
{
    return getPos() + velocity;
}

void Entity::jump()
{
    addVelocity(JUMPING_VELOCITY);
}

//teste la collision en se fiant sur la vélocité de soi-même
bool Entity::wouldBeCollidingEntityVelocity(Entity* otherEntity)
{
    const vec3 diff = glm::abs(this->getPotentialNewPos() - otherEntity->getPos());
    return diff.x <= this->hitBox->width + otherEntity->hitBox->width
        && diff.y <= this->hitBox->height + otherEntity->hitBox->height
        && diff.z <= this->hitBox->depth + otherEntity->hitBox->depth;
}

void Entity::attackEntity(Entity* attackedEntity)
{
    if (!attackedEntity->isAttackImmune)
    {
        vec3 attackingVelocity = glm::normalize(attackedEntity->getPos() - this->getPos()) / 4.0f;
        attackingVelocity.y = JUMPING_VELOCITY.y / 1.5f;
        attackedEntity->setVelocity(attackingVelocity);
        attackedEntity->getAttackedBy(this);
        attackedEntity->resetAttackImmuneTimer();
    }
}

void Entity::addReference(Entity* entityToReference)
{
    this->referencedEntities.push_back(entityToReference);
    entityToReference->referencedEntities.push_back(this);
}

//n'est PAS la méthode pour despawn, mais celle pour spawner le loot, faire le son, etc...
void Entity::die()
{
    this->isDead = true;
    this->framesUntilattackImmuneEnd = Entity::DEATH_TIMER_FRAME;
	this->setBehavior(this->getDeathBehavior());
    playDeathSound();
}

//déréférencement réciproque : on appelle la fonction ici ET chez l'autre
void Entity::removeReference(Entity* entityToDereference)
{
    this->removeReferenceSingle(entityToDereference);
    entityToDereference->removeReferenceSingle(this);
}

//déréférencement simple : pour éviter la récursivité
void Entity::removeReferenceSingle(Entity* entityToDereference)
{
    for (int i = 0 ; i < this->referencedEntities.size() ; i++)
    {
        if (this->referencedEntities[i] == entityToDereference)
        {
            this->referencedEntities[i] = this->referencedEntities.back();
            this->referencedEntities.pop_back();
            i--;
        }
    }
}

//tourne toutes les formes autour du centre de l'entité selon un axe entre normalisé entre (0, 0, 0) et (1, 1, 1)
//TODO : optimiser cette fonction pour ne pas avoir à calculer les matrices de rotation à chaque fois
void Entity::rotate(vec3 axis, float radians)
{
    //obtention de la formule de rotation d'un vecteur autour d'un axe
    glm::mat4 rotationMat = glm::rotate(mat4(1), radians, axis); //R
    originTransposition = rotationMat * originTransposition;
    for (Shape* shape : entityShapes)
    {
        shape->rotateAround(getPos(), axis, radians);
    }
    for (Cube3D* cube : entityCubes3D)
    {
        cube->rotateAround(getPos(), axis, radians);
    }
    for (Entity* e : subEntities)
    {
        e->rotateAround(getPos(), axis, radians);
    }
}

//NE PAS UTILISER POUR LES ENTITÉS À HITBOX
void Entity::rotateAround(vec3 pos, vec3 axis, float radians)
{
    //obtention de la formule de rotation d'un vecteur autour d'un axe
    glm::mat4 rotationMat = glm::rotate(mat4(1), radians, axis);
    originTransposition = rotationMat * originTransposition;
    for (Shape* shape : entityShapes)
    {
        shape->rotateAround(pos, axis, radians);
    }
    for (Cube3D* cube : entityCubes3D)
    {
        cube->rotateAround(pos, axis, radians);
    }
    for (Entity* e : subEntities)
    {
        e->rotateAround(pos, axis, radians);
    }
}

//oriente l'axe Z de l'entité sur le joueur (horizontalement)
void Entity::lookAtHorizontal(vec3 targetPos)
{
    //on fait abstraction de l'axe Y. On transpose donc les coordonnées sur un monde 2D plat.
    //obtenir le vecteur de translation de l'objet vers target, ainsi que le vecteur de direction Z orthonormé
    vec2 vecTowardsTarget2D = vec2(targetPos.x - pos.x, targetPos.z - pos.z);
    vec2 dirVec2D = vec2(getZAxis().x, getZAxis().z);

    //obtenir l'angle (horizontal) entre le vecteur de translation et celui de direction, pour savoir comment faire la rotation
    //on ne peut pas connaître le sens de cet angle, donc on devra vérifier par la suite si l'on a tourné dans la bonne direction
    float angleRadians = Shape::getAngleRadians(vecTowardsTarget2D, dirVec2D);

    if (angleRadians > 0.01f)
    {
        //[optimisation] : tester si la rotation fonctionnerait
        vec4 potentialZAxis = Shape::getVecIfRotate(getYAxis(), angleRadians, getZAxis());
        vec2 dirVec2D = vec2(potentialZAxis.x, potentialZAxis.z);

        float newAngle = Shape::getAngleRadians(vecTowardsTarget2D, dirVec2D);
        if (newAngle > 0.01f)
        {
            rotate(getYAxis(), -angleRadians);
        }else
        {
            rotate(getYAxis(), angleRadians);
        }
    }
}

//true si velocity.y == 0
bool Entity::isTouchingGround()
{
    return this->velocity.y == 0;
}

//pour les affaires qui n'ont pas de profondeur
bool Entity::isColliding(vec3 pos)
{
    for (Shape* ptrShape : this->entityShapes) if (ptrShape->isColliding(pos)) return true;
    for (Cube3D* ptrCube : this->entityCubes3D) if (ptrCube->isColliding(pos)) return true;
    for (Entity* ptrEntity : this->subEntities) if (ptrEntity->isColliding(pos)) return true;
    return false;
}

bool Entity::isColliding(Entity* otherEntity)
{
    return this->hitBox->isCollidingEntity(otherEntity->getPos(), otherEntity->hitBoxDimensions);
}

//détecte les collisions entre cubes de l'entité et de la cible
bool Entity::wouldThenBeCollidingCube(vec3 &testedVelocity, Cube3D* worldCube)
{
    //for (Cube3D* eCube : this->entityCubes3D) if (worldCube->isCollidingOtherCubeVelocity(testedVelocity, eCube)) return true;
    //for (Entity* subEntity : this->subEntities) if (subEntity->wouldThenBeCollidingCube(testedVelocity, worldCube)) return true;
    return worldCube->isCollidingOtherCubeVelocity(testedVelocity, hitBox);
}

void Entity::reportCollisionWithCubeThen(Cube3D* worldCube)
{
    glm::vec3 tryVelocityX = glm::vec3(velocity.x, 0, 0);
    glm::vec3 tryVelocityY = glm::vec3(0, velocity.y, 0);
    glm::vec3 tryVelocityZ = glm::vec3(0, 0, velocity.z);

    if (wouldThenBeCollidingCube(tryVelocityX, worldCube)) collisionLog.x++;
    if (wouldThenBeCollidingCube(tryVelocityY, worldCube)) collisionLog.y++;
    if (wouldThenBeCollidingCube(tryVelocityZ, worldCube)) collisionLog.z++;
}

//permet d'obtenir l'équivalent d'un axe du monde NON NORMALISÉ, selon les coordonnées locales de la forme
vec3 Entity::getLocalEquivalent(vec3 axis)
{
    return vec3(originTransposition * vec4(axis, 1.0f));
}

vec3 Entity::getXAxis()
{
    return vec3(originTransposition * Shape::AXIS_X);
    //return dirVecX;
}

vec3 Entity::getYAxis()
{
    return vec3(originTransposition * Shape::AXIS_Y);
    //return dirVecY;
}

vec3 Entity::getZAxis()
{
    return vec3(originTransposition * Shape::AXIS_Z);
    //return dirVecZ;
}

glm::vec3& Entity::getPos()
{
    return pos;
}

float Entity::getPos(int i)
{
    return pos[i];
}

void Entity::resetAttackImmuneTimer()
{
    this->isAttackImmune = true;
    this->framesUntilattackImmuneEnd = getAttackImmuneFrameConst();
}

void Entity::initEntity()
{

}

//À REFÉDINIR OPTIONNELLEMENT
int Entity::getAttackImmuneFrameConst()
{
    return ATTACK_COOLDOWN_FRAME;
}

int Entity::getDefaultHealth()
{
    return DEFAULT_HEALTH;
}

void Entity::getAttackedBy(Entity* attacker)
{
    this->health--;
    setRed(true);
}

void Entity::doAnimation()
{
    cout << "\n\nERREUR : fonction Entity::doAnimation() non redéfinie dans la classe enfant!\n\n";
}

void Entity::Delete()
{
    for (Entity* referencedEntity : this->referencedEntities)
    {
        removeReference(referencedEntity);
    }

    for (Entity* subEntity : this->subEntities)
    {
        subEntity->Delete();
    }
    for (Shape* subShape : this->entityShapes)
    {
        delete(subShape);
    }
    for (Cube3D* subCube : this->entityCubes3D)
    {
        delete(subCube);
    }
    delete(this);
}

function<void(void)> Entity::getDeathBehavior()
{
    return [this](){
        if (this->isTouchingGround())
        {
            setVelocity(vec3(0));
        }
        doAnimation();
    };
}

//rotationne avec un dégradé dépendamment du timer d'invincibilité
void Entity::doDeathAnimation()
{
    const vec3 rotPoint = getPos() - vec3(0, hitBoxDimensions.y/2, 0);
    const vec3 rotAxis = glm::normalize(getZAxis());  
    const float DEG90 = M_PI / 2;		
	this->rotateAround(rotPoint, rotAxis, DEG90 / DEATH_TIMER_FRAME);
}

//FONCTIONS À REDÉFINIR OBLIGATOIREMENT_______________________________

void Entity::playDeathSound()
{
    cout << "\n\nERREUR : fonction Entity::playDeathSound() non redéfinie dans la classe enfant!\n\n";
}

function<void(void)> Entity::getDefaultClassBehavior()
{
    cout << "\n\nERREUR : fonction Entity::getDefaultEntityBehavior() non redéfinie dans la classe enfant!\n\n";
    return [](){};
}