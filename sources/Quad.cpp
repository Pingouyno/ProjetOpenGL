#include"../headers/Quad.h"

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

void Quad::initQuad(glm::vec3 pos, float width, float height, glm::vec3 color, Texture* tex, Axis axis)
{
    this->pos = pos;
    this->width = width;
    this->height = height;
    this->color = color;
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

//utile pour le texte car ajuste le ratio Y selon les proportions de la texture
Quad::Quad(vec3 pos, Texture* tex, float realWidth, Axis axis)
{
    initQuad(pos, realWidth, realWidth * ((float)tex->heightImg / tex->widthImg), DEFAULT_COLOR, tex, axis);
}

Quad::Quad(glm::vec3 pos, float width, float height, Texture* tex, Axis axis)
{
    initQuad(pos, width, height, DEFAULT_COLOR, tex, axis);
}

Quad::Quad(glm::vec3 pos, float size, Texture* tex, Axis axis)
{
    initQuad(pos, size, size, DEFAULT_COLOR, tex, axis);
}

Quad::Quad(glm::vec3 pos, float width, float height, glm::vec3 color, Axis axis)
{
    initQuad(pos, width, height, color, nullptr, axis);
}


Quad::Quad(glm::vec3 pos, float size, glm::vec3 color, Axis axis)
{
    initQuad(pos, size, size, color, nullptr, axis);
}

Quad::Quad(glm::vec3 pos, float size, Axis axis)
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
    }else
    {
        glDrawElements(GL_TRIANGLES, INDICE_COUNT, GL_UNSIGNED_INT, (void*)(indexInIndices * sizeof(int)));
    }
}

//va bugguer si la taille d'une coordonnée est de 0
void Quad::resize(float width, float height)
{   
    float scaleX = width / this->width - 1;
    float scaleY = height / this->height - 1;

    for (int i = 0 ; i < shapeVertices.size(); i += 3)
    {
        shapeVertices[i] += scaleX * (shapeVertices[i] - pos[0]);
        shapeVertices[i + 1] += scaleY * (shapeVertices[i + 1] - pos[1]);
        shapeVertices[i + 2] += scaleX * (shapeVertices[i + 2] - pos[2]);
    }
    this->width = width;
    this->height = height;

    refreshGLVertices();
}

void Quad::setAxis(Axis axis)
{
    this->axis = axis;
    initVertices();
}

bool Quad::isColliding(vec3 &camPos)
{
    const float MIN_DISTANCE = 0.01f;

    float distX = abs(pos[0] - camPos[0]);
    float distY = abs(pos[1] - camPos[1]);
    float distZ = abs(pos[2] - camPos[2]);

    switch(axis){
        case X:
            return (distX <= MIN_DISTANCE / 2.0f
                && distY <= (width + MIN_DISTANCE) / 2.0f
                && distZ <= (height + MIN_DISTANCE) / 2.0f);
            break;

        case Y:
            return (distX <= (height + MIN_DISTANCE) / 2.0f
                && distY <= MIN_DISTANCE / 2.0f
                && distZ <= (width + MIN_DISTANCE) / 2.0f);
            break;

        case Z:
            return (distX <= (height + MIN_DISTANCE) / 2.0f
                && distY <= (width + MIN_DISTANCE) / 2.0f
                && distZ <= MIN_DISTANCE / 2.0f);
            break;
    }
    return false;
}

bool Quad::isCollidingEntity(vec3 &entityPos, vec3 &entityDimensions)
{
    float distX = abs(pos[0] - entityPos[0]);
    float distY = abs(pos[1] - entityPos[1]);
    float distZ = abs(pos[2] - entityPos[2]);

    switch(axis){
        case X:
            return (distX <= entityDimensions.x / 2.0f
                && distY <= (width + entityDimensions.y) / 2.0f
                && distZ <= (height + entityDimensions.z) / 2.0f);
            break;

        case Y:
            return (distX <= (height + entityDimensions.x) / 2.0f
                && distY <= entityDimensions.y / 2.0f
                && distZ <= (width + entityDimensions.z) / 2.0f);
            break;

        case Z:
            return (distX <= (height + entityDimensions.x) / 2.0f
                && distY <= (width + entityDimensions.y) / 2.0f
                && distZ <= entityDimensions.z / 2.0f);
            break;
    }
    return false;
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
                x,       y + height, z,          //1 Top left   
                x,       y + height, z + width,  //2 Top right   
                x,       y,          z + width   //3 Bottom right
            };
            for (int i = 0 ; i < shapeVertices.size() ; i+= 3) 
            {
                shapeVertices[i + 1] -= height / 2.0f;
                shapeVertices[i + 2] -= width / 2.0f;
            }
            break;

        case Y:
            shapeVertices = 
            {
                x,          y,        z,          //0 Bottom left    
                x,          y,        z + height, //1 Top left   
                x + width,  y,        z + height, //2 Top right   
                x + width,  y,        z           //3 Bottom right
            };
            for (int i = 0 ; i < shapeVertices.size() ; i+= 3) 
            {
                shapeVertices[i + 0] -= width / 2.0f;
                shapeVertices[i + 2] -= width / 2.0f;
            }
            break;

        case Z:
            shapeVertices = 
            {
                x,          y,          z,       //0 Bottom left    
                x,          y + height, z,       //1 Top left   
                x + width,  y + height, z,       //2 Top right   
                x + width,  y,          z        //3 Bottom right
            };
            for (int i = 0 ; i < shapeVertices.size() ; i+= 3) 
            {
                shapeVertices[i + 0] -= width / 2.0f;
                shapeVertices[i + 1] -= height / 2.0f;
            }
            break;

        //Équivaut à Axis::Z sans le forEach. les quad2D ont des coordonnées qui commencent en bas à gauche plutôt qu'au milieu
        case OVERLAY:
            shapeVertices = 
            {
                x,          y,          z,       //0 Bottom left    
                x,          y + height, z,       //1 Top left   
                x + width,  y + height, z,       //2 Top right   
                x + width,  y,          z        //3 Bottom right
            };
            break;
    }
}