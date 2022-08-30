#ifndef CUBE_3D_CLASS_H
#define CUBE_3D_CLASS_H

#include"Cube.h"

//Pour une cubemap : https://matheowis.github.io/HDRI-to-CubeMap/
/*Pour flip la texture posx (px) : alias 'flip=convert -flip' (on fait flip in.jpg out.jpg)
  -> https://askubuntu.com/questions/1052899/how-to-mirror-image-files-via-command-line
  -> pour se rendre dans le répertoire TEXTURES : **goTexture** (alias) */

class Cube3D : public Cube {
    protected:
        const static vector<float> NORMAL_COORDS;  

    public:
        static int VERTICE_COUNT;
        static int INDICE_COUNT;

        Cube3D(glm::vec3 pos, glm::vec3 dimensions, Texture* tex);
        Cube3D(void* dummy);
        void render();
        int getVerticeCount();
        int getIndiceCount();
        void setToBackground();

    protected:
        void replaceColorWithTexCoords();
        void initIndices();
        void initVertices();
};
#endif