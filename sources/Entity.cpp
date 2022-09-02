#include"../headers/Entity.h"

const float Entity::RADIAN_CIRCLE = 2*M_PI;

Entity::Entity(glm::vec3 pos)
{
    this->pos = pos;
    this->behavior = [](){};
    this->entityShapes = {};
    this->entityCubes3D = {};
    this->subEntities = {};
    this->dirFacing = DEFAULT_DIRECTION;
    this->active = true;
    this->originTransposition = mat4(1.0f);
    this->velocity = vec3(0);
    this->hitBox = nullptr;
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

void Entity::doBehavior()
{
    this->behavior();
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
    hitBox->moveTo(hitBox->pos + (newPos - getPos()));
    setPos(newPos);
}

void Entity::setVelocity(vec3 velocity)
{
    this->velocity = velocity;
    for (Entity* ptrEntity : subEntities)
    {
        ptrEntity->setVelocity(velocity);
    }
}

void Entity::addVelocity(vec3 velocityToAdd)
{
    this->velocity += velocityToAdd;
    for (Entity* ptrEntity : subEntities)
    {
        ptrEntity->addVelocity(velocityToAdd);
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
    moveTo(getPos() + velocity);
}

vec3 Entity::getPotentialNewPos()
{
    return getPos() + velocity;
}

void Entity::jump()
{
    velocity.y = 0.12f;
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
    glm::mat4 rotationMat = glm::rotate(mat4(1), radians, axis); //R
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
    hitBox->rotateAround(pos, axis, radians);
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

//pour les affaires qui n'ont pas de profondeur
bool Entity::isColliding(vec3 pos)
{
    for (Shape* ptrShape : this->entityShapes) if (ptrShape->isColliding(pos)) return true;
    for (Cube3D* ptrCube : this->entityCubes3D) if (ptrCube->isColliding(pos)) return true;
    for (Entity* ptrEntity : this->subEntities) if (ptrEntity->isColliding(pos)) return true;
    return false;
}

//détecte les collisions entre cubes de l'entité et de la cible
bool Entity::wouldThenBeCollidingCube(vec3 &testedVelocity, Cube3D* worldCube)
{
    //for (Cube3D* eCube : this->entityCubes3D) if (worldCube->isCollidingOtherCubeVelocity(testedVelocity, eCube)) return true;
    //for (Entity* subEntity : this->subEntities) if (subEntity->wouldThenBeCollidingCube(testedVelocity, worldCube)) return true;
    return worldCube->isCollidingOtherCubeVelocity(testedVelocity, hitBox);
}

void Entity::reportCollisionWithCubeThen(vec3 &collisionLog, Cube3D* worldCube)
{
    glm::vec3 tryVelocityX = glm::vec3(velocity.x, 0, 0);
    glm::vec3 tryVelocityY = glm::vec3(0, velocity.y, 0);
    glm::vec3 tryVelocityZ = glm::vec3(0, 0, velocity.z);

    if (wouldThenBeCollidingCube(tryVelocityX, worldCube)) collisionLog[0]++;
    if (wouldThenBeCollidingCube(tryVelocityY, worldCube)) collisionLog[1]++;
    if (wouldThenBeCollidingCube(tryVelocityZ, worldCube)) collisionLog[2]++;
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

//FONCTIONS À REDÉFINIR OBLIGATOIREMENT_______________________________

void Entity::doAnimation()
{
    cout << "\n\nERREUR : fonction Entity::doAnimation() non redéfinie dans la classe enfant!\n\n";
}

function<void(void)> Entity::getDefaultClassBehavior()
{
    cout << "\n\nERREUR : fonction Entity::getDefaultEntityBehavior() non redéfinie dans la classe enfant!\n\n";
    return [](){};
}

void Entity::setPos(glm::vec3 newPos)
{
    this->pos = newPos;
}