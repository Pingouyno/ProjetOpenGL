#include<vector>

#include"../headers/Shape.h"

enum Shape::Type : char { PHYSICAL = 'p', OVERLAY = 'o', HUD_STATIC = 's', HUD_COLLIDE = 'c'};

const vec3 Shape::ROT_X = vec3(1, 0, 0);
const vec3 Shape::ROT_Y = vec3(0, 1, 0);
const vec3 Shape::ROT_Z = vec3(0, 0, 1);

glm::vec3 Shape::DEFAULT_COLOR({1.0f, 0.5f, 0.5f});
std::vector<float> Shape::DEFAULT_TEXMAP({0.0f, 1.0f});
std::vector<float> Shape::NO_TEXMAP({0.0f, 0.0f});

//ces valeurs sont divisées par deux car on calculer les colissions devant et derrière (diamètre plutôt que rayon)
float Shape::camBoxHeight = 2.8f;
float Shape::camBoxWidth = 0.4f;

Shape::Shape(){
    shouldReloadArrays = true;
    //TODO: changer la direction de base pour certaines classes de Quad (sinon on ne les voit pas ; deviennent parallèles à la caméra)
    direction = vec3(0, 0, 1);
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

//tourne toujours dans le sens horaire
void Shape::rotate(vec3 axis, float radians)
{
    //pour rotation autour d'un point (x, y, z), avec T = translate et R = rotate : T(x,y,z) * R * T(-x,-y,-z) 
    //applique règle de la main droite : on multiplie le vecteur à gauche pour un sens, et à droite pour l'auter sens
   
    //translation de l'origine de 3 vecteurs orthonormés sur pos
    mat4 translateMat = glm::translate(mat4(1), pos); //T(x, y, z)
    mat4 minusTranslateMat = glm::translate(mat4(1), -pos); //T(-x, -y, -z)

    //obtention de la formule de rotation d'un vecteur autour d'un axe
    glm::mat4 rotationMat = glm::rotate(mat4(1), radians, axis); //R

    for (int i = 0 ; i < shapeVertices.size(); i+=3)
    {
        vec3 currentVertice(shapeVertices[i], shapeVertices[i+1], shapeVertices[i+2]);
        //multiplication de la coordonnée par la formule pour la rotation 
        currentVertice = glm::vec3(translateMat * rotationMat * minusTranslateMat * glm::vec4(currentVertice, 1.0));
        shapeVertices[i] = currentVertice[0];
        shapeVertices[i + 1] = currentVertice[1];
        shapeVertices[i + 2] = currentVertice[2];
    }
    //on translate le vecteur de direction sur l'origin, puis on le rétablit à l'origine en soustrayant pos
    direction = glm::vec3(translateMat * rotationMat * minusTranslateMat * glm::vec4(direction + pos, 1.0)) - pos;
    refreshGLVertices();
}

//angle = arccos[(xa * xb + ya * yb) / (√(xa2 + ya2) * √(xb2 + yb2))]
//on sait que glm::length(vec2 a) = √(xa^2 + ya^2)
/*https://stackoverflow.com/questions/5467007/inverting-rotation-in-3d-to-make-an-object-always-face-the-camera/5487981#5487981
 la fonctioon lookAt est faite pour la caméra, et donc elle fait une rotation entière du monde autour de la caméra.
 On utilise une formule qui pratique la rotation de lookAt, sans avoir le déplacement (comme ça la forme tourne sur elle même)*/
void Shape::lookAt(vec3 targetPos)
{
    bool shouldRotateAgain;
    do{
        vec3 vecTowardTarget = targetPos - pos;
        float formula = (direction.x * vecTowardTarget.x + direction.z * vecTowardTarget.z) / (glm::length(vec2(direction.x, direction.z)) * glm::length(vec2(vecTowardTarget.x, vecTowardTarget.z)));
        float angleRadians = acos(formula);  
        shouldRotateAgain = angleRadians > 0.01f;
        if (shouldRotateAgain) 
        {
            rotate(Shape::ROT_Y, (angleRadians));
        }
    }while(shouldRotateAgain);

    /*
    cout << "relativePos : " << vecTowardTarget[0] << " | " << vecTowardTarget[1] << " | " << vecTowardTarget[2]<< "\n";
    cout << "direction : "<< direction[0] << " | " << direction[1] << " | " << direction[2]<< "\n";
    cout << "distance : " << glm::length(vec2(vecTowardTarget.x, vecTowardTarget.z)) << "\n";
    cout << "formula : " << formula << "\n";
    cout <<  "angleRadian : " << angleRadians << "\n";
    cout <<  "angleDegrees : " << radiansToDegrees(angleRadians) << "\n\n\n";
    */
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

void Shape::printMat4(mat4 &mat)
{
    cout << "\nmat4 : {";
    float* matArr = glm::value_ptr(mat);
    for (int i = 0 ; i < mat.length(); i++)
    {
        cout << "\n\t";
        for (int j = 0 ; j < 4; j++)
        {
            cout << matArr[4*i + j] << ", ";
        }
    }
    cout << "\n};\n";
}

/*https://stackoverflow.com/questions/5467007/inverting-rotation-in-3d-to-make-an-object-always-face-the-camera/5487981#5487981
 la fonctioon lookAt est faite pour la caméra, et donc elle fait une rotation entière du monde autour de la caméra.
 On utilise une formule qui pratique la rotation de lookAt, sans avoir le déplacement (comme ça la forme tourne sur elle même)
void Shape::lookAt(vec3 targetPos)
{
    vec3 front = glm::normalize(pos - targetPos);
    vec3 right = normalize(cross(Shape::ROT_Y, front));

    vec3 up = cross(front, right);
    mat4 transformation
               (right.x, up.x, front.x, pos.x,
                right.y, up.y, front.y, pos.y,
                right.z, up.z, front.z, pos.z,
                0,       0,    0,       1);
    
    for (int i = 0 ; i < shapeVertices.size(); i+=3)
    {
        vec3 currentVertice(shapeVertices[i], shapeVertices[i+1], shapeVertices[i+2]);
        
        //multiplication de la coordonnée par la formule pour la rotation 
        currentVertice = glm::vec3(transformation * glm::vec4(currentVertice, 1.0));

        shapeVertices[i] = currentVertice[0];
        shapeVertices[i + 1] = currentVertice[1];
        shapeVertices[i + 2] = currentVertice[2];
    }
    refreshGLVertices();
}
*/
