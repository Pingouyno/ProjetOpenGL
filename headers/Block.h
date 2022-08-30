#ifndef BLOCK_CLASS_H
#define BLOCK_CLASS_H

#include"Cube3D.h"

class Block : public Cube3D
{
    public:
        const static float BLOCK_SIZE;
        int indexInRendering;

        Block(glm::vec3 pos, Texture* tex);

        bool isInRenderingVec();
        void uploadDataIntoVerticesAndIndices();
        void generate();
};
#endif