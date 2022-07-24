#include"../headers/Shape.h"
#include"../headers/Cube.h"
#include<vector>
#include"../headers/Camera.h"

std::vector<float> Shape::DEFAULT_COLOR({1.0f, 0.5f, 0.5f});
std::vector<float> Shape::DEFAULT_TEXMAP({0.0f, 1.0f});
std::vector<float> Shape::NO_TEXMAP({0.0f, 0.0f});
vector<Shape*> Shape::shapes({});
bool Shape::newShapeCreated = false;

void Shape::spawn()
{
    cout << "\n\n**ATTENTION : FONCTION \"Shape::spawn()\" NON REDÉFINIE!**\n\n";
}

void Shape::despawn()
{
    cout << "\n\n**ATTENTION : FONCTION \"Shape::despawn()\" NON REDÉFINIE!**\n\n";
}

void Shape::moveTo(float x, float y, float z)
{
    cout << "\n\n**ATTENTION : FONCTION \"Shape::moveTo()\" NON REDÉFINIE!**\n\n";
}

const void Shape::moveTo(float (&pos)[3])
{
    moveTo(pos[0], pos[1], pos[2]);
}

void Shape::render()
{
    cout << "\n\n**ATTENTION : FONCTION \"Shape::render()\" NON REDÉFINIE!**\n\n";
}

bool Shape::isColliding(Camera Camera)
{
    cout << "\n\n**ATTENTION : FONCTION \"Shape::isColliding()\" NON REDÉFINIE!**\n\n";
    return false;
}

void Shape::setSize(float size)
{
    cout << "\n\n**ATTENTION : FONCTION \"Shape::setSize()\" NON REDÉFINIE!**\n\n";
}

bool Shape::hasTexture()
{
    return this->tex != nullptr;
}

//Render toutes les entités, et désactive "newShapeCreated"
void Shape::renderActiveShapes()
{
    for (Shape* ptrShape : shapes)
    {
        if ((*ptrShape).active)
        {
            (*ptrShape).render();
        }
    }

    newShapeCreated = false;
}

Shape* Shape::checkCameraCollidingAnyShape(Camera camera)
{
    for (Shape* ptrShape : shapes)
    {
        if ((*ptrShape).active && (*ptrShape).isColliding(camera))
        {
            return ptrShape;
        }
    }
    return nullptr;
}