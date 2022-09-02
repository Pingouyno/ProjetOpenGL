#include"../headers/Cube3D.h"

/*map que l'on insère dans la place de color dans vertices[], qui est utilisée par le shader pour savoir 
 quelle face est couremment dessinée. les vertices respectent le même schéma que ce cube.*/
const vector<float> Cube3D::NORMAL_COORDS = 
{
    //   coordonnées
    -1.0f, -1.0f,  1.0f, //       7--------6
    1.0f, -1.0f,  1.0f,  //      /|       /|
    1.0f, -1.0f, -1.0f,  //     4--------5 |
    -1.0f, -1.0f, -1.0f, //     | |      | |
    -1.0f,  1.0f,  1.0f, //     | 3------|-2
    1.0f,  1.0f,  1.0f,  //     |/       |/
    1.0f,  1.0f, -1.0f,  //     0--------1
    -1.0f,  1.0f, -1.0f
};

//car on se fie à texMap[1] pour brightness
const vector<float> Cube3D::BRIGHTNESS_TEXMAP = 
{
    0, 1, //0       7--------6
    0, 1, //1      /|       /|
    0, 1, //2     4--------5 |
    0, 1, //3     | |      | |
    0, 1, //4     | 3------|-2
    0, 1, //5     |/       |/
    0, 1, //6     0--------1
    0, 1  //7
};

int Cube3D::VERTICE_COUNT = 8;   //Taille du tableau 3 fois plus grosse que 8 car un vertex a (x, y, z)
int Cube3D::INDICE_COUNT = 36;    //2 triangles par côté, 6 côtés par cube = 3 * 2 * 6 = 36  

/*Texture* vaut (Texture*)1 pour ne pas avoir de nullptr dans la classe parent
 on appeller Cube(void* dummy) car la fonction generate() doît être appelée dans la même classe qui a initVertices() et initIndice()*/
Cube3D::Cube3D(glm::vec3 pos, glm::vec3 dimensions, Texture* tex) : Cube(nullptr)
{   
    this->pos = pos;
    this->width = dimensions.x;
    this->height = dimensions.y;
    this->depth = dimensions.z;
    this->color = DEFAULT_COLOR;
    this->tex = tex;

    //important à cause du shader, qui se fie à texMap[1] pour la luminosité
    this->texMap = BRIGHTNESS_TEXMAP;

    Shape::generate(); 
    replaceColorWithTexCoords();
}

Cube3D::Cube3D(void* dummy) : Cube(nullptr){}

void Cube3D::render()
{
    tex->Bind();
    glDrawElements(GL_TRIANGLES, getIndiceCount(), GL_UNSIGNED_INT, (void*)(indexInIndices * sizeof(int)));
    tex->Unbind();
}

int Cube3D::getVerticeCount()
{
    return VERTICE_COUNT;
}

int Cube3D::getIndiceCount()
{
    return INDICE_COUNT;
}

/* met texMap[0] = 2.0f pour tous les vertex (regarder le shader pour plus de détails). 
   Permet d'indiquer au shaderProgramCube que l'on veut dessiner le cube en background (skybox). 
   La taille d'une cube en background n'est pas importante car le shader tracera comme si il est à la distance maximale*/
void Cube3D::setToBackground()
{
    //itérer à travers les 8 vertex
    for (int i = 0 ; i < getVerticeCount() ; i++){
        //6 =offset de texMap[0] dans layout de vertex
        vertices[indexInVertices + 8*i + 6] = 2.0f;
    }  
}

bool Cube3D::isCollidingOtherCubeVelocity(vec3 &velocity, Cube3D* otherCube)
{
    const vec3 otherPos = otherCube->pos + velocity;

    float distX = abs(pos[0] - otherPos[0]);
    float distY = abs(pos[1] - otherPos[1]);
    float distZ = abs(pos[2] - otherPos[2]);

    return (distX <= (this->width + otherCube->width) / 2.0f
        && distY <= (this->height + otherCube->height) / 2.0f
        && distZ <= (this->depth + otherCube->depth) / 2.0f);
}

/*remplace l'espace réservé à vec3 color par les coordonnées normalisées du cube, afin que le shader qui render la cubemap
 puisse savoir quelle face il trace actuellement*/
void Cube3D::replaceColorWithTexCoords()
{
    int cpt = 0;
    //itérer à travers les 8 vertex
    for (int i = 0 ; i < getVerticeCount() ; i++){
        //itérer à travers les 3 coordonnées de NORMAL_COORDS
        for (int j = 0 ; j < 3 ; j++) 
        {
            /*-> indexInVertices = début de vertices pour cette forme,
              -> 8*i = décalement de 8 fois vertex courant (car vertex est de taille 8 floats dans tableau) 
              -> 3 = offset de COLOR  [vec3 pos|vec3 color|vec2 tex]
              -> j = coordonnées courante (x, y ou z)*/
        
            vertices[indexInVertices + 8*i + 3 + j] = NORMAL_COORDS[cpt];
            cpt++;
        }
    }
}

//appelé dans Generate()
void Cube3D::initIndices()
{
    int i = indexInVertices / 8;
    Shape::shapeIndices =
    {
        // Right
        i+1, i+2, i+6,
        i+6, i+5, i+1,
        // Left
        i, i+4, i+7,
        i+7, i+3, i+0,
        // Top
        i+4, i+5, i+6,
        i+6, i+7, i+4,
        // Bottom
        i+0, i+3, i+2,
        i+2, i+1, i+0,
        // Back
        i+0, i+1, i+5,
        i+5, i+4, i+0,
        // Front
        i+3, i+7, i+6,
        i+6, i+2, i+3
    };
}

void Cube3D::initVertices()
{  
    float x = pos[0];
    float y = pos[1];
    float z = pos[2];

    //regarder NORMAL_COORDS pour référence
    Shape::shapeVertices = 
    {
        x,         y,          z + depth, //0       7--------6   
        x + width, y,          z + depth, //1      /|       /|  
        x + width, y,          z,         //2     4--------5 |
        x,         y,          z,         //3     | |      | |
        x,         y + height, z + depth, //4     | 3------|-2
        x + width, y + height, z + depth, //5     |/       |/    
        x + width, y + height, z,         //6     0--------1
        x,         y + height, z          //7     
    };
    
    //centrer le cube sur pos, plutôt que de commencer sur pos
    for (int i = 0 ; i < shapeVertices.size() ; i+= 3) 
    {
        shapeVertices[i + 0] -= width / 2.0f;
        shapeVertices[i + 1] -= height / 2.0f;
        shapeVertices[i + 2] -= depth / 2.0f;
    }
}

