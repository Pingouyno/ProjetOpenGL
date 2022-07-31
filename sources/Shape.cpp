#include"../headers/Cube.h"
#include"../headers/Quad2D.h"
#include<vector>
#include"../headers/Camera.h"
#include"../headers/GlobalArrays.h"

std::vector<float> Shape::DEFAULT_COLOR({1.0f, 0.5f, 0.5f});
std::vector<float> Shape::DEFAULT_TEXMAP({0.0f, 1.0f});
std::vector<float> Shape::NO_TEXMAP({0.0f, 0.0f});
vector<Shape*> Shape::shapes({});
vector<Shape*> Shape::shapes2D({});
bool Shape::shouldReloadArrays = false;
float Shape::screenHeight = -1;
float Shape::screenWidth = -1;

float Shape::camBoxHeight = 1.4f;
float Shape::camBoxWidth = 0.2f;


//**DÉBUT FONCTIONS D'HÉRITAGE VIRTUELLES**

void Shape::render(){printUndefinedErr("RENDER");}
void Shape::resize(float &size){printUndefinedErr("RESIZE");}
bool Shape::isColliding(glm::vec3 &camPos){printUndefinedErr("COLLIDING"); return false;}
int Shape::getVerticeCount(){printUndefinedErr("VERTICECOUNT"); return 0;}
int Shape::getIndiceCount(){printUndefinedErr("INDICECOUNT"); return 0;}
vector<float> Shape::getShapeTexMap(){printUndefinedErr("TEXMAP"); return DEFAULT_TEXMAP;}
void Shape::initIndices(){printUndefinedErr("INITINDICE");}
void Shape::initVertices(){printUndefinedErr("INITVERTICE");}


//**FIN FONCTIONS D'HÉRITAGE VIRTUELLES**

/*IMPORTANT : les coordonnées de pixel de la souris commmencent à (0, 0) en haut à gauche, 
 tandis que les coordonnées de ratio commencent à (0.0f, 0.0f) au MILLIEU de l'écran*/

void Shape::initVariables(float screenWidth, float screenHeight)
{
    Shape::screenWidth = screenWidth;
    Shape::screenHeight = screenHeight;
}

/*transforme une longueur de pixel en ratio d'écran horizontal. 
 ne peut PAS être utilisé pour la translation de coordonnées, car OpenGL a le milieu de l'écran à (0.0)*/
float Shape::toXRatio(float pixSize)
{
    return 2.0f * pixSize / Shape::screenWidth;
}

/*transforme une longueur de pixel en ratio d'écran vertical. 
 ne peut PAS être utilisé pour la translation de coordonnées, car OpenGL a le milieu de l'écran à (0.0)*/
float Shape::toYRatio(float pixSize)
{
    return 2.0f * pixSize / Shape::screenHeight;
}

/*transforme une coordonnée ratio openGL (entre -0.5 et 0.5) en coordonnée de pixel de souris. 
 ne peut PAS être utilisé pour la translation de longueur, car openGL a le milieu de l'écran à (0.0f, 0.0f)
 rappel : l'origine de la souris est en HAUT À GAUCHE 
        : l'origine de openGL est au MILIEU mais commence en BAS À GAUCHE (-1, -1)*/
 
float Shape::toXPixelCoord(float xRatio)
{
    return (xRatio + 1.0f) / 2.0f * Shape::screenWidth;
}

float Shape::toYPixelCoord(float yRatio)
{
    return (1.0f - ((yRatio + 1.0f) / 2.0f)) * Shape::screenHeight;
}


//Render toutes les entités, et désactive "newShapeCreated"
void Shape::renderActive3DShapes()
{
    for (Shape* ptrShape : shapes)
    {
        if ((*ptrShape).active)
        {
            (*ptrShape).render();
        }
    }
    shouldReloadArrays = false;
}

void Shape::renderActive2DShapes()
{
    for (Shape* ptrShape : shapes2D)
    {
        if ((*ptrShape).active)
        {
            (*ptrShape).render();
        }
    }
    shouldReloadArrays = false;
}

void Shape::renderActiveHUDShapes()
{
    for (Quad2D* ptrShape : Quad2D::shapesHUD)
    {
        if (ptrShape->active)
        {
            ptrShape->render();
        }
    }
    shouldReloadArrays = false;
}

void Shape::checkCameraCollidingAnyHUD(glm::vec3 &mousePos)
{
    for (Quad2D* ptrShape : Quad2D::shapesHUD)
    {
        if (ptrShape->active && ptrShape->isColliding(mousePos))
        {
            ptrShape->doClickLogic();
            
            /*on échappe car on ne cliquer qu'un seul icône à la fois.
             de plus, si un des boutons réactive le suivant dans la liste, 
             on peut avoir un comportement non défini (ré-activation des boutons perpétuelle)*/
            break;
        }
    }

}

vector<int> Shape::checkCameraCollidingAnyShape(glm::vec3 &oldPos, glm::vec3 &newPos)
{
    vector<int> collisionLog({0, 0, 0});
    for (Shape* ptrShape : shapes)
    {
        if ((*ptrShape).active && (*ptrShape).isColliding(newPos))
            (*ptrShape).reportCollision(collisionLog, oldPos, newPos);
    }
    return collisionLog;
}

bool Shape::isAnyColliding(vector<int> &collisionLog)
{
    return collisionLog[0] != 0 || collisionLog[1] != 0 || collisionLog[2] != 0;
}

void Shape::addShape(Type shapeType, Shape* shape)
{
    switch(shapeType)
    {
        case PHYSICAL:
            shapes.push_back(shape);
            break;

        case OVERLAY:
            shapes2D.push_back(shape);
            break;

        case HUD:
            Quad2D::shapesHUD.push_back((Quad2D*)shape);
            break;
    }
}

void Shape::deleteAllShapes()
{
    int i = shapes.size() - 1;
    while(i >= 0)
    {
        shapes.erase(shapes.begin() + i);
        i--;
    }
    vertices.clear();
    indices.clear();
    shouldReloadArrays = true;
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

void Shape::reportCollision(vector<int> &collisionLog, glm::vec3 &oldPos, glm::vec3 &newPos)
{
    glm::vec3 tryPosX = glm::vec3(newPos[0], oldPos.y, oldPos.z);
    glm::vec3 tryPosY = glm::vec3(oldPos.x, newPos[1], oldPos.z);
    glm::vec3 tryPosZ = glm::vec3(oldPos.x, oldPos.y, newPos[2]);

    if (isColliding(tryPosX)) collisionLog[0]++;
    if (isColliding(tryPosY)) collisionLog[1]++;
    if (isColliding(tryPosZ)) collisionLog[2]++;
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