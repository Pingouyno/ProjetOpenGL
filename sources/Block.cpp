#include"../headers/Block.h"

const float Block::BLOCK_SIZE = 1.0f;

Block::Block(glm::vec3 pos, Texture* tex) : Cube3D(nullptr)
{
    this->indexInRendering = -1;
    Cube::initCube(pos, BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, DEFAULT_COLOR, tex);
    this->active = true;    
}

//retourne true si le cube est compris dans l'array de cubes à render
bool Block::isInRenderingVec()
{
    return this->indexInRendering != -1;
}

//override la focntion de Shape() car on veut gagner de la performance sur le remplacement de couleur
void Block::uploadDataIntoVerticesAndIndices()
{
    //resize au besoin
    if (indexInVertices == vertices.size()) vertices.resize(vertices.size() + getVerticeCount() * 8);
    if (indexInIndices == indices.size()) indices.resize(indices.size() + getIndiceCount());

    const int offset = indexInVertices;

    //upload dans vertices
    const int verticeCount = getVerticeCount();
    for (int v = 0 ; v < verticeCount ; v++){
        const int base = offset + v*8;
        vertices[base + 0] = shapeVertices[v*3 + 0];
        vertices[base + 1] = shapeVertices[v*3 + 1];
        vertices[base + 2] = shapeVertices[v*3 + 2];

        vertices[base + 3] = NORMAL_COORDS[v*3 + 0];
        vertices[base + 4] = NORMAL_COORDS[v*3 + 1];
        vertices[base + 5] = NORMAL_COORDS[v*3 + 2];

        vertices[base + 6] = texMap[v*2 + 0];
        vertices[base + 7] = texMap[v*2 + 1];
    }
    
    //upload dans indices
    const int indiceCount = getIndiceCount();
    for (int i = 0 ; i < indiceCount; i++)
    {
        indices[indexInIndices + i] = shapeIndices[i];
    }
}

//upload dans le buffer de openGL
void Block::generate()
{
    initIndices();         
    initVertices();

    uploadDataIntoVerticesAndIndices();

    //s'il faut l'aggrandir alors pas besoin de reload le subData
    reloadVerticesInVBO();
    reloadIndicesInEBO();

}