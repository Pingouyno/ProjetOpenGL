#include"../headers/Quad.h"
#include"../headers/GlobalArrays.h"

int Quad::VERTICE_COUNT = 4;

int Quad::INDICE_COUNT = 6;  //2 triangles

vector<float> Quad::SHAPE_TEXMAP(
    {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
    }
);

void Quad::initQuad(vector<float> &pos, float &length, float &width, vector<float> &color, Texture* tex, Axis axis)
{
    this->pos.insert(this->pos.end(), pos.begin(), pos.end());
    this->width = width;
    this->length = length;
    this->color.insert(this->color.end(), color.begin(), color.end());
    this->tex = tex;
    this->axis = axis;
    if (this->tex == nullptr)
        //remplire la texmap avec le motif qui indique de ne pas render de texture, chaque vertice
        for(int i = 0 ; i < VERTICE_COUNT ; i++)
            texMap.insert(texMap.end(), NO_TEXMAP.begin(), NO_TEXMAP.end());
    else
        texMap.insert(texMap.end(), SHAPE_TEXMAP.begin(), SHAPE_TEXMAP.end());
    generate(); 
}

Quad::Quad(vector<float> &pos, float &size, Texture* tex, Axis axis)
{
    initQuad(pos, size, size, DEFAULT_COLOR, tex, axis);
}

Quad::Quad(vector<float> &pos, float &size, vector<float> &color, Axis axis)
{
    initQuad(pos, size, size, color, nullptr, axis);
}

Quad::Quad(vector<float> &pos, float &size, Axis axis)
{
    initQuad(pos, size, size, DEFAULT_COLOR, nullptr, axis);
}

int Quad::getVerticeCount()
{
    return VERTICE_COUNT;
}

int Quad::getIndiceCount()
{
    return INDICE_COUNT;
}

vector<float> Quad::getShapeTexMap()
{
    return SHAPE_TEXMAP;
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

void Quad::resize(float &size)
{
    resize(size, size);
}

void Quad::resize(float &length, float &width)
{
    this->length = length;
    this->width = width;
    initVertices();
    refreshGLVertices();
}

bool Quad::isColliding(Camera &camera)
{
    float distX = pos[0] - camera.Position[0];
    float distY = pos[1] - camera.Position[1];
    float distZ = pos[2] - camera.Position[2];

    bool colliding;

    switch(axis){
        case X:
            return (distX >= -camera.hitBoxWidth && distX <= camera.hitBoxWidth
                && distY >= -(length + camera.hitBoxHeight) && distY <= 0
                && distZ >= -(width + camera.hitBoxWidth) && distZ <= camera.hitBoxWidth);
            break;

        case Y:
            return (distX >= -(width + camera.hitBoxWidth) && distX <= camera.hitBoxWidth
                && distY >= -camera.hitBoxHeight && distY <= 0
                && distZ >= -(length + camera.hitBoxWidth) && distZ <= camera.hitBoxWidth);
            break;

        case Z:
            return (distX >= -(width + camera.hitBoxWidth) && distX <= camera.hitBoxWidth
                && distY >= -(length + camera.hitBoxHeight) && distY <= 0
                && distZ >= -camera.hitBoxWidth && distZ <= camera.hitBoxWidth);
            break;
    }

    return colliding;
}

void Quad::reportCollision(vector<int> &collisionLog, Camera &camera)
{
    float distX = pos[0] - camera.Position[0];
    float distY = pos[1] - camera.Position[1];
    float distZ = pos[2] - camera.Position[2];

    switch(axis){
        case X:
            if (distX >= -camera.hitBoxWidth && distX <= camera.hitBoxWidth) collisionLog[0]++;
            if (distY >= -(length + camera.hitBoxHeight) && distY <= 0) collisionLog[1]++;
            if (distZ >= -(width + camera.hitBoxWidth) && distZ <= camera.hitBoxWidth) collisionLog[2]++;
            break;

        case Y:
            if (distX >= -(width + camera.hitBoxWidth) && distX <= camera.hitBoxWidth) collisionLog[0]++;
            if (distY >= -camera.hitBoxHeight && distY <= 0) collisionLog[1]++;
            if (distZ >= -(length + camera.hitBoxWidth) && distZ <= camera.hitBoxWidth) collisionLog[2]++;;
            break;

        case Z:
            if (distX >= -(width + camera.hitBoxWidth) && distX <= camera.hitBoxWidth) collisionLog[0]++;
            if (distY >= -(length + camera.hitBoxHeight) && distY <= 0) collisionLog[1]++;
            if (distZ >= -camera.hitBoxWidth && distZ <= camera.hitBoxWidth) collisionLog[2]++;
            break;
    }
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

    switch(axis)
    {
        case X:
            //                   **Selon la perspective de l'axe en question**
            shapeVertices = 
            {
                x,       y,          z,          //0 Bottom left    
                x,       y + length, z,          //1 Top left   
                x,       y + length, z + width,  //2 Top right   
                x,       y,          z + width   //3 Bottom right
            };
            break;

        case Y:
            shapeVertices = 
            {
                x,         y,        z,          //0 Bottom left    
                x,         y,        z + length, //1 Top left   
                x + width, y,        z + length, //2 Top right   
                x + width, y,        z           //3 Bottom right
            };
            break;

        case Z:
            shapeVertices = 
            {
                x,         y,          z,       //0 Bottom left    
                x,         y + length, z,       //1 Top left   
                x + width, y + length, z,       //2 Top right   
                x + width, y,          z        //3 Bottom right
            };
            break;
    }
}