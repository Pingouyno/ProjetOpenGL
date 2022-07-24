#include"../headers/Cube.h"
#include"../headers/GlobalArrays.h"
#include<GLFW/glfw3.h>

//coordonnées de texture pour chacune des 8 vertices
const vector<float> Cube::CUBE_TEXMAP(
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

/*
    x,        y,        z,         //0 Bottom near left    
    x,        y,        z + size,  //1 Bottom far left   
    x + size, y,        z + size,  //2 Bottom far right   
    x + size, y,        z,         //3 Bottom near right
    x,        y + size, z,         //4 Top near left    
    x,        y + size, z + size,  //5 Top far left   
    x + size, y + size, z + size,  //6 Top far right   
    x + size, y + size, z          //7 Top near right
*/

void Cube::initCube(vector<float> &pos, float &size, vector<float> &color, Texture* tex)
{
    this->pos.insert(this->pos.end(), pos.begin(), pos.end());
    this->size = size;
    this->color.insert(this->color.end(), color.begin(), color.end());
    this->tex = tex;
    if (this->tex == nullptr)
        //remplire la texmap avec le motif qui indique de ne pas render de texture, chaque vertice
        for(int i = 0 ; i < VERTICE_COUNT ; i++)
            texMap.insert(texMap.end(), NO_TEXMAP.begin(), NO_TEXMAP.end());
    else
        texMap.insert(texMap.end(), CUBE_TEXMAP.begin(), CUBE_TEXMAP.end());
    shapes.push_back(this);
    generate(); 
}

Cube::Cube(vector<float> &pos, float &size, Texture* tex)
{
    initCube(pos, size, DEFAULT_COLOR, tex);
}

Cube::Cube(vector<float> &pos, float &size, vector<float> &color)
{
    initCube(pos, size, color, nullptr);
}

Cube::Cube(vector<float> &pos, float &size)
{
    initCube(pos, size, DEFAULT_COLOR, nullptr);
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

    return (distX >= -size && distX <= 0
        && distY >= -(size + camera.hitBoxHeight) && distY <= 0
        && distZ >= -size && distZ <= 0);
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
        x,        y,        z,         //0 Bottom near left    
        x,        y,        z + size,  //1 Bottom far left   
        x + size, y,        z + size,  //2 Bottom far right   
        x + size, y,        z,         //3 Bottom near right

        x,        y + size, z,         //4 Top near left    
        x,        y + size, z + size,  //5 Top far left   
        x + size, y + size, z + size,  //6 Top far right   
        x + size, y + size, z,         //7 Top near right

       //ces coordonnées sont nécessaires pour le texture mapping
        x,        y + size, z,         //8  Top near left (copie 4)    
        x,        y + size, z + size,  //9  Top far left (copie 5)  
        x + size, y + size, z + size,  //10 Top far right (copie 6)  
        x + size, y + size, z          //11 Top near right (copie 7)
    };
}

void Cube::refreshGLVertices()
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

void Cube::generate()
{
    //i = index du tableau, en numérotation de chaque vertex dans vertices[]
    this -> indexInVertices = vertices.size();  //chaque vertex contient (x, y, z) + (r, g, b) + (mapX, mapY) = 8
    this -> indexInIndices = indices.size();

    initIndices();         //init indices avant pour trouver index de début d'insertion vertices
    initVertices();

    for (int i = 0 ; i < VERTICE_COUNT ; i++){
        vertices.insert(vertices.end(), shapeVertices.begin() + 3 * i, shapeVertices.begin() + 3 * i + 3);
        vertices.insert(vertices.end(), color.begin(), color.end());
        vertices.insert(vertices.end(), texMap.begin() + 2 * i, texMap.begin() + 2 * i + 2);
    }
    indices.insert(indices.end(), shapeIndices.begin(), shapeIndices.end());
    active = true;
    spawn();
}
