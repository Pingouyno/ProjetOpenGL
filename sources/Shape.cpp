#include<vector>

#include"../headers/Shape.h"

enum Shape::Type : char { PHYSICAL = 'p', OVERLAY = 'o', HUD_STATIC = 's', HUD_COLLIDE = 'c'};
//dans le sens des aiguilles d'une montre (horizontal), la tête vers le bas (vertical)
enum Shape::Rotation : char { HORIZONTAL = 'h', VERTICAL = 'v'};

glm::vec3 Shape::DEFAULT_COLOR({1.0f, 0.5f, 0.5f});
std::vector<float> Shape::DEFAULT_TEXMAP({0.0f, 1.0f});
std::vector<float> Shape::NO_TEXMAP({0.0f, 0.0f});
float Shape::camBoxHeight = 1.4f;
float Shape::camBoxWidth = 0.2f;

Shape::Shape(){
    shouldReloadArrays = true;
}

//**DÉBUT FONCTIONS D'HÉRITAGE VIRTUELLES**

void Shape::render(){printUndefinedErr("RENDER");}
void Shape::resize(float width, float height){printUndefinedErr("RESIZE");}
bool Shape::isColliding(glm::vec3 &camPos){printUndefinedErr("COLLIDING"); return false;}
int Shape::getVerticeCount(){printUndefinedErr("VERTICECOUNT"); return 0;}
int Shape::getIndiceCount(){printUndefinedErr("INDICECOUNT"); return 0;}
vector<float> Shape::getShapeTexMap(){printUndefinedErr("TEXMAP"); return DEFAULT_TEXMAP;}
void Shape::initIndices(){printUndefinedErr("INITINDICE");}
void Shape::initVertices(){printUndefinedErr("INITVERTICE");}


//**FIN FONCTIONS D'HÉRITAGE VIRTUELLES**

/*IMPORTANT : les coordonnées de pixel de la souris commmencent à (0, 0) en haut à gauche, 
 tandis que les coordonnées de ratio commencent à (0.0f, 0.0f) au MILLIEU de l'écran*/

/*transforme une longueur de pixel en ratio d'écran horizontal. 
 ne peut PAS être utilisé pour la translation de coordonnées, car OpenGL a le milieu de l'écran à (0.0)*/
float Shape::toXRatio(float pixSize)
{
    return 2.0f * pixSize / screenWidth;
}

/*transforme une longueur de pixel en ratio d'écran vertical. 
 ne peut PAS être utilisé pour la translation de coordonnées, car OpenGL a le milieu de l'écran à (0.0)*/
float Shape::toYRatio(float pixSize)
{
    return 2.0f * pixSize / screenHeight;
}

/*transforme une coordonnée ratio openGL (entre -0.5 et 0.5) en coordonnée de pixel de souris. 
 ne peut PAS être utilisé pour la translation de longueur, car openGL a le milieu de l'écran à (0.0f, 0.0f)
 rappel : l'origine de la souris est en HAUT À GAUCHE 
        : l'origine de openGL est au MILIEU mais commence en BAS À GAUCHE (-1, -1)*/
 
float Shape::toXPixelCoord(float xRatio)
{
    return (xRatio + 1.0f) / 2.0f * screenWidth;
}

float Shape::toYPixelCoord(float yRatio)
{
    return (1.0f - ((yRatio + 1.0f) / 2.0f)) * screenHeight;
}

void Shape::spawn()
{
    active = true;
}

void Shape::despawn()
{
    active = false;
}

void Shape::moveTo(float x, float y, float z)
{
    float diffX = x - pos[0];
    float diffY = y - pos[1];
    float diffZ = z - pos[2]; 
    for (int i = 0; i < shapeVertices.size(); i += 3)
    {
        shapeVertices[i] += diffX;
        shapeVertices[i + 1] += diffY;
        shapeVertices[i + 2] += diffZ;
    }
    pos = vec3(x, y, z);
    refreshGLVertices();
}

void Shape::moveTo(glm::vec3 pos)
{
    moveTo(pos[0], pos[1], pos[2]);
}

void Shape::resize(float size)
{
    resize(size, size);
}

void Shape::rotate(float degrees, Rotation rotationDir)
{
    if (rotationDir == Rotation::HORIZONTAL)
    {
        
    }else if (rotationDir == Rotation::VERTICAL)
    {

    }
    refreshGLVertices();
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
        for (int i = 0 ; i < 3 ; i++) vertices.push_back(color[i]);
        vertices.insert(vertices.end(), texMap.begin() + 2 * i, texMap.begin() + 2 * i + 2);
    }
    indices.insert(indices.end(), shapeIndices.begin(), shapeIndices.end());
    spawn();
}

void Shape::printUndefinedErr(string funcName)
{
    cout << "\n\n**ERREUR : fonction <" << funcName << "> non redéfinie hors de Shape**\n\n";
}