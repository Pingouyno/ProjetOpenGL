#include"../headers/Shape.h"
#include"../headers/Cube.h"
#include<vector>
#include"../headers/Camera.h"
#include"../headers/GlobalArrays.h"

std::vector<float> Shape::DEFAULT_COLOR({1.0f, 0.5f, 0.5f});
std::vector<float> Shape::DEFAULT_TEXMAP({0.0f, 1.0f});
std::vector<float> Shape::NO_TEXMAP({0.0f, 0.0f});
vector<Shape*> Shape::shapes({});
bool Shape::newShapeCreated = false;

//**DÉBUT FONCTIONS D'HÉRITAGE VIRTUELLES**

void Shape::render(){printUndefinedErr("RENDER");}
bool Shape::isColliding(Camera &camera){printUndefinedErr("COLLIDING"); return false;}
void Shape::resize(float &size){printUndefinedErr("RESIZE");}
int Shape::getVerticeCount(){printUndefinedErr("VERTICECOUNT"); return 0;}
int Shape::getIndiceCount(){printUndefinedErr("INDICECOUNT"); return 0;}
vector<float> Shape::getShapeTexMap(){printUndefinedErr("TEXMAP"); return DEFAULT_TEXMAP;}
void Shape::initIndices(){printUndefinedErr("INITINDICE");}
void Shape::initVertices(){printUndefinedErr("INITVERTICE");}

//**FIN FONCTIONS D'HÉRITAGEVIRTUELLES**

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

Shape* Shape::checkCameraCollidingAnyShape(Camera &camera)
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

void Shape::addShape(Shape* shape)
{
    shapes.push_back(shape);
}

void Shape::deleteAllShapes()
{
    for (Shape* shape : shapes) delete(shape);
    vertices.clear();
    indices.clear();
}

void Shape::spawn()
{
    active = true;
}

void Shape::despawn()
{
    active = false;
}

void Shape::moveTo(float &x, float &y, float &z)
{
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
    initVertices();
    refreshGLVertices();
}

void Shape::moveTo(float (&pos)[3])
{
    moveTo(pos[0], pos[1], pos[2]);
}

bool Shape::hasTexture()
{
    return this->tex != nullptr;
}

void Shape::refreshGLVertices()
{
    //vertices[]
    int i = indexInVertices;
    //CUBE_VERTICES[]
    int c = 0;
    
    while (c < shapeVertices.size())
    {
        for (int inc = 0 ; inc < 3 ; inc++)
        {
            vertices[i + inc] = shapeVertices[c + inc];
        }      

        i += 8; //on passe au prochain vertex dans vertices[]
        c += 3; //on passe au prochain (x, y, z) dans CUBE_VERTICES
    }
}

void Shape::generate()
{
    //i = index du tableau, en numérotation de chaque vertex dans vertices[]
    this -> indexInVertices = vertices.size();  //chaque vertex contient (x, y, z) + (r, g, b) + (mapX, mapY) = 8
    this -> indexInIndices = indices.size();

    initIndices();         //init indices avant pour trouver index de début d'insertion vertices
    initVertices();

    for (int i = 0 ; i < getVerticeCount() ; i++){
        vertices.insert(vertices.end(), shapeVertices.begin() + 3 * i, shapeVertices.begin() + 3 * i + 3);
        vertices.insert(vertices.end(), color.begin(), color.end());
        vertices.insert(vertices.end(), texMap.begin() + 2 * i, texMap.begin() + 2 * i + 2);
    }
    indices.insert(indices.end(), shapeIndices.begin(), shapeIndices.end());
    spawn();
}