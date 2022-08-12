#include"../headers/Entity.h"

const float Entity::RADIAN_CIRCLE = 2*M_PI;

Entity::Entity(glm::vec3 pos)
{
    this->pos = pos;
    this->behavior = [](){};
    this->entityShapes = {};
    this->dirFacing = DEFAULT_DIRECTION;
    this->active = true;
    this->originTransposition = mat4(1.0f);
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
}

void Entity::doBehavior()
{
    this->behavior();
}

void Entity::setBehavior(function<void(void)> behavior)
{
    this->behavior = behavior;
}

void Entity::addShape(Shape* ptrShape)
{
    entityShapes.push_back(ptrShape);
}

void Entity::moveTo(glm::vec3 newPos)
{
    for (Shape* ptrShape : entityShapes)
    {
        //déplacer les éléments de l'équivalent de la différence (x, y, z) entre newPos et oldPos
        ptrShape->moveTo(ptrShape->pos + (newPos - getPos()));
    }
    setPos(newPos);
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

void Entity::setPos(glm::vec3 &newPos)
{
    this->pos = newPos;
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