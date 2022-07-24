#ifndef QUAD_CLASS_H
#define QUAD_CLASS_H

#include"Shape.h"
#include"Texture.h"

class Quad: public Shape {
    public:

        static int VERTICE_COUNT;
        static int INDICE_COUNT; 
        static vector<float> SHAPE_TEXMAP;

        float length;
        float width;

        void initQuad(vector<float> &pos, float &length, float &width, vector<float> &color, Texture* tex);
        Quad(vector<float> &pos, float &size, Texture* texture);
        Quad(vector<float> &pos, float &size, vector<float> &color);
        Quad(vector<float> &pos, float &size);

        void spawn();
        void despawn();
        void moveTo(float &x, float &y, float &z);
        void render();
        bool isColliding(Camera &camera);
        void setSize(float &length, float &width);
        void setSize(float &size);
    
    private:
        void initIndices();
        void initVertices();
        void refreshGLVertices();
        void generate();
};

#endif