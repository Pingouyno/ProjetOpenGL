#include"../headers/Cube.h"

int Cube::VERTICE_COUNT = 12;   //Taille du tableau 3 fois plus grosse que 8 car un vertex a (x, y, z)
int Cube::INDICE_COUNT = 36;    //2 triangles par côté, 6 côtés par cube = 3 * 2 * 6 = 36  

//coordonnées de texture pour chacune des 8 vertices
vector<float> Cube::SHAPE_TEXMAP(
    {
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,

        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,

        //correctifs pour texture mapping
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f
    }
);

void Cube::initCube(glm::vec3 pos, float width, float height, float depth, glm::vec3 color, Texture* tex)
{
    this->pos = pos;
    this->width = width;
    this->height = height;
    this->depth = depth;
    this->color = color;
    this->tex = tex;
    if (this->tex == nullptr)
        //remplire la texmap avec le motif qui indique de ne pas render de texture, chaque vertice
        for(int i = 0 ; i < getVerticeCount() ; i++)
            texMap.insert(texMap.end(), NO_TEXMAP.begin(), NO_TEXMAP.end());
    else
        texMap.insert(texMap.end(), SHAPE_TEXMAP.begin(), SHAPE_TEXMAP.end());
    generate(); 
}

//dimensions : x = width, y = height, z = depth
Cube::Cube(glm::vec3 pos, glm::vec3 dimensions, Texture* tex)
{
    initCube(pos, dimensions.x, dimensions.y, dimensions.z, DEFAULT_COLOR, tex);
}

Cube::Cube(glm::vec3 pos, float width, float height, float depth, glm::vec3 color)
{
    initCube(pos, width, height, depth, color, nullptr);
}

Cube::Cube(glm::vec3 pos, float width, float height, Texture* tex)
{
    initCube(pos, width, height, width, DEFAULT_COLOR, tex);
}

Cube::Cube(glm::vec3 pos, float width, float height, glm::vec3 color)
{
    initCube(pos, width, height, width, color, nullptr);
}

Cube::Cube(glm::vec3 pos, float size, Texture* tex)
{
    initCube(pos, size, size, size, DEFAULT_COLOR, tex);
}

Cube::Cube(glm::vec3 pos, float size, glm::vec3 color)
{
    initCube(pos, size, size, size, color, nullptr);
}

Cube::Cube(glm::vec3 pos, float size)
{
    initCube(pos, size, size, size, DEFAULT_COLOR, nullptr);
}

int Cube::getVerticeCount()
{
    return VERTICE_COUNT;
}

int Cube::getIndiceCount()
{
    return INDICE_COUNT;
}

vector<float> Cube::getShapeTexMap()
{
    return SHAPE_TEXMAP;
}

void Cube::render()
{
    if (hasTexture())
    {
        (*tex).Bind();
        glDrawElements(GL_TRIANGLES, getIndiceCount(), GL_UNSIGNED_INT, (void*)(indexInIndices * sizeof(int)));
        (*tex).Unbind();
    }else glDrawElements(GL_TRIANGLES, getIndiceCount(), GL_UNSIGNED_INT, (void*)(indexInIndices * sizeof(int)));
}

void Cube::resize(float width, float height)
{   
    resize(width, depth, width);
}

void Cube::resize(float width, float height, float depth)
{   
    float scaleX = width / this->width - 1;
    float scaleY = height / this->height - 1;
    float scaleZ = depth / this->depth - 1;

    for (int i = 0 ; i < shapeVertices.size(); i += 3)
    {
        shapeVertices[i] += scaleX * (shapeVertices[i] - pos[0]);
        shapeVertices[i + 1] += scaleY * (shapeVertices[i + 1] - pos[1]);
        shapeVertices[i + 2] += scaleZ * (shapeVertices[i + 2] - pos[2]);
    }
    this->width = width;
    this->height = height;
    this->depth = depth;
    refreshGLVertices();
}

bool Cube::isColliding(glm::vec3 &camPos)
{
    float distX = abs(pos[0] - camPos[0]);
    float distY = abs(pos[1] - camPos[1]);
    float distZ = abs(pos[2] - camPos[2]);

    return (distX <= (width + camBoxWidth) / 2.0f
        && distY <= (height + camBoxHeight) / 2.0f
        && distZ <= (depth + camBoxWidth) / 2.0f);
}

void Cube::initIndices()
{
    //Divise par huit pour obtenir une numérotation de vertex plutot qu'indice de tableau
    int i = indexInVertices / 8;

    shapeIndices = 
    {
        i,   i+1, i+3,
        i+1, i+3, i+2, //bottom

        i+5, i+4, i+6,
        i+4, i+6, i+7, //top

        i,   i+3, i+4,
        i+3, i+4, i+7, //front

        i+2, i+1, i+6,
        i+1, i+6, i+5, //back

        i,   i+1, i+8,
        i+1, i+8, i+9, //left

        i+2, i+3, i+10,
        i+3, i+10,i+11  //right
    };
}

void Cube::initVertices()
{
    float x = pos[0];
    float y = pos[1];
    float z = pos[2];

    shapeVertices = 
    {
        x,         y,        z,          //0 Bottom near left    
        x,         y,        z  + depth,  //1 Bottom far left   
        x + width, y,        z  + depth,  //2 Bottom far right   
        x + width, y,        z,          //3 Bottom near right

        x,         y + height, z,           //4 Top near left    
        x,         y + height, z  + depth,  //5 Top far left   
        x + width, y + height, z  + depth,  //6 Top far right   
        x + width, y + height, z,           //7 Top near right

       //ces coordonnées sont nécessaires pour le texture mapping
        x,         y + height, z,           //8  Top near left (copie 4)    
        x,         y + height, z  + depth,  //9  Top far left (copie 5)  
        x + width, y + height, z  + depth,  //10 Top far right (copie 6)  
        x + width, y + height, z            //11 Top near right (copie 7)
    };

    //centrer le cube sur pos, plutôt que de commencer sur pos
    for (int i = 0 ; i < shapeVertices.size() ; i+= 3) 
    {
        shapeVertices[i + 0] -= width / 2.0f;
        shapeVertices[i + 1] -= height / 2.0f;
        shapeVertices[i + 2] -= depth / 2.0f;
    }
}