#include"../headers/Quad.h"
#include"../headers/GlobalArrays.h"

int Quad::VERTICE_COUNT = 4;

int Quad::INDICE_COUNT = 6;  //2 triangles

vector<float> Quad::SHAPE_TEXMAP(
    {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
    }
);

void Quad::initQuad(vector<float> &pos, float &length, float &width, vector<float> &color, Texture* tex)
{
    this->pos.insert(this->pos.end(), pos.begin(), pos.end());
    this->width = width;
    this->length = length;
    this->color.insert(this->color.end(), color.begin(), color.end());
    this->tex = tex;
    if (this->tex == nullptr)
        //remplire la texmap avec le motif qui indique de ne pas render de texture, chaque vertice
        for(int i = 0 ; i < VERTICE_COUNT ; i++)
            texMap.insert(texMap.end(), NO_TEXMAP.begin(), NO_TEXMAP.end());
    else
        texMap.insert(texMap.end(), SHAPE_TEXMAP.begin(), SHAPE_TEXMAP.end());
    shapes.push_back(this);
    generate(); 
}

Quad::Quad(vector<float> &pos, float &size, Texture* tex)
{
    initQuad(pos, size, size, DEFAULT_COLOR, tex);
}

Quad::Quad(vector<float> &pos, float &size, vector<float> &color)
{
    initQuad(pos, size, size, color, nullptr);
}

Quad::Quad(vector<float> &pos, float &size)
{
    initQuad(pos, size, size, DEFAULT_COLOR, nullptr);
}


void Quad::spawn()
{
    active = true;
}

void Quad::despawn()
{
    active = false;
}

void Quad::moveTo(float &x, float &y, float &z)
{
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
    initVertices();
    refreshGLVertices();
}

void Quad::render()
{
    if (hasTexture())
    {
        (*tex).Bind();
        glDrawElements(GL_TRIANGLES, INDICE_COUNT, GL_UNSIGNED_INT, (void*)(indexInIndices * sizeof(int)));
        (*tex).Unbind();
    }else glDrawElements(GL_TRIANGLES, INDICE_COUNT, GL_UNSIGNED_INT, (void*)(indexInIndices * sizeof(int)));
}

bool Quad::isColliding(Camera &camera)
{
    float distX = pos[0] - camera.Position[0];
    float distY = pos[1] - camera.Position[1];
    float distZ = pos[2] - camera.Position[2];

    return (distX >= -width && distX <= 0
        && distY >= -camera.hitBoxHeight && distY <= 0
        && distZ >= -length && distZ <= 0);
}

void Quad::setSize(float &size)
{
    setSize(size, size);
}

void Quad::setSize(float &length, float &width)
{
    this->length = length;
    this->width = width;
    initVertices();
    refreshGLVertices();
}

//**fonctions privées**

void Quad::initIndices()
{
    //Divise par huit pour obtenir une numérotation de vertex plutot qu'indice de tableau
    int i = indexInVertices / 8;

    shapeIndices = 
    {
        i+1, i,   i+2,
        i,   i+2, i+3 
    };
}

void Quad::initVertices()
{

    float x = pos[0];
    float y = pos[1];
    float z = pos[2];

    shapeVertices = 
    {
        x,         y,        z,          //0 Bottom near left    
        x,         y,        z + length, //1 Bottom far left   
        x + width, y,        z + length, //2 Bottom far right   
        x + width, y,        z           //3 Bottom near right
    };
}

void Quad::refreshGLVertices()
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


void Quad::generate()
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
    spawn();
}