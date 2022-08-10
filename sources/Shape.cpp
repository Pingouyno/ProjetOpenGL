#include<vector>

#include"../headers/Shape.h"

enum Shape::Type : char { PHYSICAL = 'p', OVERLAY = 'o', HUD_STATIC = 's', HUD_COLLIDE = 'c'};

const vec3 Shape::ROT_X = vec3(1, 0, 0);
const vec3 Shape::ROT_Y = vec3(0, 1, 0);
const vec3 Shape::ROT_Z = vec3(0, 0, 1);

const vec4 Shape::AXIS_X = vec4(ROT_X, 1.0f);
const vec4 Shape::AXIS_Y = vec4(ROT_Y, 1.0f);
const vec4 Shape::AXIS_Z = vec4(ROT_Z, 1.0f);

glm::vec3 Shape::DEFAULT_COLOR({1.0f, 0.5f, 0.5f});
std::vector<float> Shape::DEFAULT_TEXMAP({0.0f, 1.0f});
std::vector<float> Shape::NO_TEXMAP({0.0f, 0.0f});

//ces valeurs sont divisées par deux car on calculer les colissions devant et derrière (diamètre plutôt que rayon)
float Shape::camBoxHeight = 2.8f;
float Shape::camBoxWidth = 0.4f;

Shape::Shape(){
    shouldReloadArrays = true;
    //TODO: changer la direction de base pour certaines classes de Quad (sinon on ne les voit pas ; deviennent parallèles à la caméra)
    originTransposition = mat4(1.0f);
    dirVecX = ROT_X;
    dirVecY = ROT_Y;
    dirVecZ = ROT_Z;
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

/*angle = arccos[(xa * xb + ya * yb) / (√(xa2 + ya2) * √(xb2 + yb2))]
 on sait que glm::length(vec2 a) = √(xa^2 + ya^2)*/
float Shape::getAngleRadians(vec2 a, vec2 b)
{
    return acos(
             (a.x * b.x + a.y * b.y) 
      / (glm::length(a) * glm::length(b))
    );  
}

float Shape::getAngleRadians(vec3 a, vec3 b)
{
    return acos(
        (a.x * b.x + a.y * b.y + a.z * b.z) 
      / (glm::length(a) * glm::length(b))
    );  
}

//Permet d'obtenir le vecteur si l'on appliquait une rotation sur celui-ci.
vec4 Shape::getVecIfRotate(vec3 axis, float radians, vec3 vectorToRotate)
{
    glm::mat rotationMat = glm::rotate(mat4(1), radians, axis); //R
    return rotationMat * vec4(vectorToRotate, 1);
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

//tourner sur soi-même, avec comme repère d'axe l'origine du monde (0, 0, 0) -> (1, 1, 1)
void Shape::rotate(vec3 axis, float radians)
{
   rotateAround(pos, axis, radians);
}

//tourne autour d'un point selon un axe entre (0, 0, 0) et (1, 1, 1)
void Shape::rotateAround(vec3 centerPos, vec3 axis, float radians)
{
    //pour rotation autour d'un point (x, y, z), avec T = translate et R = rotate : T(x,y,z) * R * T(-x,-y,-z) 
    //applique règle de la main droite : on multiplie le vecteur à gauche pour un sens, et à droite pour l'auter sens
   
    //translation de l'origine de 3 vecteurs orthonormés sur pos
    mat4 translateMat = glm::translate(mat4(1), centerPos); //T(x, y, z)
    mat4 minusTranslateMat = glm::translate(mat4(1), -centerPos); //T(-x, -y, -z)

    //obtention de la formule de rotation d'un vecteur autour d'un axe
    glm::mat4 rotationMat = glm::rotate(mat4(1), radians, axis); //R
    mat4 transformation = translateMat * rotationMat * minusTranslateMat;

    for (int i = 0 ; i < shapeVertices.size(); i+=3)
    {
        vec4 currentVertice = vec4(shapeVertices[i], shapeVertices[i+1], shapeVertices[i+2], 1.0);
        //multiplication de la coordonnée par la formule pour la rotation 
        currentVertice = transformation * currentVertice;
        shapeVertices[i] = currentVertice[0];
        shapeVertices[i + 1] = currentVertice[1];
        shapeVertices[i + 2] = currentVertice[2];
    }
    //on rotationne les coordonnées locales de la forme en appliquant la rotation sur le repère local orthonormé
    originTransposition = rotationMat * originTransposition;
    if (centerPos != pos)pos = vec3(transformation * vec4(pos, 1));
    refreshGLVertices();
}

/*Peut faire pointer l'axe Z d'un objet vers une position avec un maximum de 2 appels rotate()
 **NOTE : Un Quad doit être en axe Z pour utiliser lookAt(), car cette fonction fait pointer l'axe Z sur la position*/
void Shape::lookAt(vec3 targetPos)
{
    //obligé de reset pour s'assurer que l'axe X n'a pas été rotate, avant d'appeler lookAtVertical->(une fonction fragile)
    resetRotation();
    lookAtHorizontal(targetPos);
    lookAtVertical(targetPos);
    refreshGLVertices();
}

//oriente l'axe Z de l'objet sur le joueur (horizontalement)
void Shape::lookAtHorizontal(vec3 targetPos)
{
    //on fait abstraction de l'axe Y. On transpose donc les coordonnées sur un monde 2D plat.
    //obtenir le vecteur de translation de l'objet vers target, ainsi que le vecteur de direction Z orthonormé
    vec2 vecTowardsTarget2D = vec2(targetPos.x - pos.x, targetPos.z - pos.z);
    vec2 dirVec2D = vec2(getZAxis().x, getZAxis().z);

    //obtenir l'angle (horizontal) entre le vecteur de translation et celui de direction, pour savoir comment faire la rotation
    //on ne peut pas connaître le sens de cet angle, donc on devra vérifier par la suite si l'on a tourné dans la bonne direction
    float angleRadians = getAngleRadians(vecTowardsTarget2D, dirVec2D);

    if (angleRadians > 0.01f)
    {
        vec4 testRotatedZAxis = getVecIfRotate(getYAxis(), angleRadians, getZAxis());
        //calculer le nouvel angle. Si celui-ci n'est pas bon alors on recommence mais en faisant -2*rotation. 
        //nous permettra assurément de se centrer sur le vecteur de translation
        vec2 dirVec2D = vec2(testRotatedZAxis.x, testRotatedZAxis.z);
        float newAngle = getAngleRadians(vecTowardsTarget2D, dirVec2D);
        
        if (newAngle > 0.01f)
        {
            rotate(getYAxis(), -angleRadians);
        }else{
            rotate(getYAxis(), angleRadians);
        }
    }
}

//peut seulement être utilisé DIRECTEMENT après lookAtHorizontal (pour le moment)
//peut seulement être utilisé lorsqu'on sait que Y = 0, comme ça le vecteur de translation est normal à l'axe Y.
void Shape::lookAtVertical(vec3 targetPos)
{
    //obtenir le vecteur de translation sur la cible
    vec3 vecTowardsTarget = vec3(targetPos - pos);
    //vecteur normal au vecteur de translation  ET à l'axe X. Nous donne le nouveau vecteur que nous voulons comme orientation verticale
    //on fait "- getXAxis()" pour que le vecteur obtenu soit dans le sens inverse, et donc que (0, 0, 1) pointe sur nous plutôt que (0, 0 -1)
    vec3 crossedVector = glm::cross( - getXAxis(), vecTowardsTarget);

    float angleRadians = getAngleRadians(crossedVector, getYAxis());

    if (angleRadians > 0.01f)
    {
        vec3 rotatedXAxis = vec3(getVecIfRotate(getXAxis(), angleRadians, getXAxis()));
        vec3 rotatedYAxis = vec3(getVecIfRotate(getXAxis(), angleRadians, getYAxis()));
        
        crossedVector = glm::cross( - rotatedXAxis, vecTowardsTarget);
        float newAngle = getAngleRadians(crossedVector, rotatedYAxis);

        if (newAngle > 0.01f)
        {
           rotate(getXAxis(), -angleRadians);
        }else
        {
           rotate(getXAxis(), angleRadians);
        }
    }
}

//reset la forme à la position de départ
void Shape::resetRotation()
{
    originTransposition = mat4(1.0f);
    initVertices();
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

vec3 Shape::getXAxis()
{
    return vec3(originTransposition * AXIS_X);
    //return dirVecX;
}

vec3 Shape::getYAxis()
{
    return vec3(originTransposition * AXIS_Y);
    //return dirVecY;
}

vec3 Shape::getZAxis()
{
    return vec3(originTransposition * AXIS_Z);
    //return dirVecZ;
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

/*on utilise une approximation de PI, donc sin(degreesToRadians) ne va pas être exact. Utiliser sinf();
 nb radians / cercle = environ 2*pi */
float Shape::degreesToRadians(float degrees) 
{
    return degrees * (M_PI / 180);
}

float Shape::radiansToDegrees(float radians)
{
    return radians * 180.0f/M_PI;
}

void Shape::printUndefinedErr(string funcName)
{
    cout << "\n\n**ERREUR : fonction <" << funcName << "> non redéfinie hors de Shape**\n\n";
}