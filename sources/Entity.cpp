#include"../headers/Entity.h"

Entity::Entity(glm::vec3 pos)
{
    this->pos = pos;
    this->behavior = [](){};
    this->entityShapes = {};
    this->dirFacing = DEFAULT_DIRECTION;
    this->active = true;
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

void Entity::setDirFacing(Direction dirFacing)
{
    cout << "\n\nERREUR : fonction Entity::setDirFacing(Direction dirFacing) non redéfinie dans la classe enfant!\n\n";
}

function<void(void)> Entity::getDefaultClassBehavior()
{
    cout << "\n\nERREUR : fonction Entity::getDefaultEntityBehavior() non redéfinie dans la classe enfant!\n\n";
    return [](){};
}