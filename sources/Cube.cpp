#include"../headers/Cube.h"
#include"../headers/GlobalArrays.h"
#include<GLFW/glfw3.h>

//coordonnées de texture pour chacune des 8 vertices
const vector<float> Cube::CUBE_TEXMAP(
    {
        0.1f,
    }
);

Cube::Cube(vector<float> &pos, float &size, Texture* tex)
{
    this->pos.insert(this->pos.end(), pos.begin(), pos.end());
    this->size = size;
    this->color.insert(this->color.end(), DEFAULT_COLOR.begin(), DEFAULT_COLOR.end());
    this->tex = tex;
    texMap.insert(texMap.end(), DEFAULT_TEXMAP.begin(), DEFAULT_TEXMAP.end());
    shapes.push_back(this);
    generate(); 
}

Cube::Cube(vector<float> &pos, float &size, vector<float> &color)
{
    this->pos.insert(this->pos.end(), pos.begin(), pos.end());
    this->size = size;
    this->color.insert(this->color.end(), color.begin(), color.end());
    texMap.insert(texMap.end(), NO_TEXMAP.begin(), NO_TEXMAP.end());
    shapes.push_back(this);
    generate();
}

Cube::Cube(vector<float> &pos, float &size)
{
    this->pos.insert(this->pos.end(), pos.begin(), pos.end());
    this->size = size;
    this->color.insert(this->color.end(), DEFAULT_COLOR.begin(), DEFAULT_COLOR.end());
    texMap.insert(texMap.end(), NO_TEXMAP.begin(), NO_TEXMAP.end());
    shapes.push_back(this);
    generate();
}

void Cube::spawn()
{
    active = true;
}

void Cube::despawn()
{
    active = false;
}

void Cube::moveTo(float x, float y, float z)
{
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
    initVertices();
    refreshGLVertices();
}

void Cube::render()
{
    if (hasTexture())
    {
        (*tex).Bind();
        glDrawElements(GL_TRIANGLES, INDICE_COUNT, GL_UNSIGNED_INT, (void*)(indexInIndices * sizeof(int)));
        (*tex).Unbind();
    }else glDrawElements(GL_TRIANGLES, INDICE_COUNT, GL_UNSIGNED_INT, (void*)(indexInIndices * sizeof(int)));
}

bool Cube::isColliding(Camera camera)
{
    float distX = pos[0] - camera.Position[0];
    float distY = pos[1] - camera.Position[1];
    float distZ = pos[2] - camera.Position[2];

    return (distX >= 0 && distX <= size
        && distY >= -camera.hitBoxHeight && distY <= size
        && distZ >= 0 && distZ <= size);
}

void Cube::setSize(float size)
{
    this->size = size;
    initVertices();
    refreshGLVertices();
}

void Cube::initIndices()
{
    //Divise par huit pour obtenir une numérotation de vertex plutot qu'indice de tableau
    int i = indexInVertices / 8;

    CUBE_INDICES = 
    {
        i,   i+1, i+2,
        i,   i+2, i+3, //bottom

        i+4, i+5, i+6,
        i+4, i+6, i+7, //top

        i,   i+3, i+4,
        i+3, i+4, i+7, //front

        i+1, i+6, i+5,
        i+1, i+2, i+6, //back

        i,   i+1, i+5,
        i+5, i+0, i+4, //left

        i+3, i+2, i+6,
        i+6, i+3, i+7  //right
    };
}

void Cube::initVertices()
{

    float x = pos[0];
    float y = pos[1];
    float z = pos[2];

    CUBE_VERTICES = 
    {
        x,        y,        z,         //0 Bottom near left    
        x,        y,        z - size,  //1 Bottom far left   
        x - size, y,        z - size,  //2 Bottom far right   
        x - size, y,        z,         //3 Bottom near right
        x,        y - size, z,         //4 Top near left    
        x,        y - size, z - size,  //5 Top far left   
        x - size, y - size, z - size,  //6 Top far right   
        x - size, y - size, z          //7 Top near right
    };
}

void Cube::refreshGLVertices()
{
    //vertices[]
    int i = indexInVertices;
    //CUBE_VERTICES[]
    int c = 0;
    
    while (c < CUBE_VERTICES.size())
    {
        for (int inc = 0 ; inc < 3 ; inc++)
        {
            vertices[i + inc] = CUBE_VERTICES[c + inc];
        }      

        i += 8; //on passe au prochain vertex dans vertices[]
        c += 3; //on passe au prochain (x, y, z) dans CUBE_VERTICES
    }
}

void Cube::generate()
{
    //i = index du tableau, en numérotation de chaque vertex dans vertices[]
    this -> indexInVertices = vertices.size();  //chaque vertex contient (x, y, z) + (r, g, b) + (mapX, mapY) = 8
    this -> indexInIndices = indices.size();

    initIndices();         //init indices avant pour trouver index de début d'insertion vertices
    initVertices();

    for (int i = 0 ; i < VERTICE_COUNT ; i++){
        vertices.insert(vertices.end(), CUBE_VERTICES.begin() + 3 * i, CUBE_VERTICES.begin() + 3 * i + 3);
        vertices.insert(vertices.end(), color.begin(), color.end());
        vertices.insert(vertices.end(), texMap.begin(), texMap.end());
    }
    indices.insert(indices.end(), CUBE_INDICES.begin(), CUBE_INDICES.end());
    active = true;
    spawn();
}
