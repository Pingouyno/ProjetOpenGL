#ifndef QUAD_CLASS_H
#define QUAD_CLASS_H

#include"Shape.h"
#include"Texture.h"

class Quad: public Shape {
    public:

        static int VERTICE_COUNT;
        static int INDICE_COUNT; 
        static vector<float> SHAPE_TEXMAP;

        //axe sur lequel on peut "enfiler en brochette" le quad ; pour une plateforme horizontale on a 'Y'.
        enum Axis : char { X = 'x', Y = 'y', Z = 'z'};

        float length;
        float width;
        Axis axis;

        void initQuad(vector<float> &pos, float &length, float &width, vector<float> &color, Texture* tex, Axis axis);
        Quad(vector<float> &pos, float &size, Texture* texture, Axis axis);
        Quad(vector<float> &pos, float &size, vector<float> &color, Axis axis);
        Quad(vector<float> &pos, float &size, Axis axis);

        int getVerticeCount();
        int getIndiceCount();
        vector<float> getShapeTexMap();

        void render();
        void resize(float &length, float &width);
        void resize(float &size);
        bool isColliding(glm::vec3 &camPos);
    
    private:
        void initIndices();
        void initVertices();
};

#endif