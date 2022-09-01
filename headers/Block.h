#ifndef BLOCK_CLASS_H
#define BLOCK_CLASS_H

#include"Cube3D.h"

class Block : public Cube3D
{
    private:
        const static float BLOCK_BRIGHTNESS;
        const static float FACE_BRIGHTNESS;
        //texmap où pour chaque vertice, 
        static vector<float> BLOCK_TEXMAP;

    public:

        const static float BLOCK_SIZE;
        int indexInRendering;

        Block(glm::vec3 pos, Texture* tex);

        bool isInRenderingVec();
        void uploadDataIntoVerticesAndIndices();
        void generate();
};
#endif