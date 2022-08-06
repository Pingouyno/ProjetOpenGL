#ifndef QUAD_CLASS_H
#define QUAD_CLASS_H

#include"Shape.h"

class Quad: public Shape {
    public:

        static int VERTICE_COUNT;
        static int INDICE_COUNT; 
        static vector<float> SHAPE_TEXMAP;

        //axe sur lequel on peut "enfiler en brochette" le quad ; pour une plateforme horizontale on a 'Y'.
        enum Axis : char { X = 'x', Y = 'y', Z = 'z', NONE = '?'};

        float width;
        float height;

        Axis axis;

        void initQuad(glm::vec3 pos, float width, float height, glm::vec3 color, Texture* tex, Axis axis);
        Quad(glm::vec3 pos, float width, float height, Texture* tex, Axis axis);
        Quad(glm::vec3 pos, float size, Texture* texture, Axis axis);
        Quad(glm::vec3 pos, float size, glm::vec3 color, Axis axis);
        Quad(glm::vec3 pos, float size, Axis axis);
        Quad(){}

        int getVerticeCount();
        int getIndiceCount();
        vector<float> getShapeTexMap();

        void render();
        void resize(float &width, float &heigth);
        void resize(float &size);
        bool isColliding(glm::vec3 &camPos);
    
    private:
        void initIndices();
        void initVertices();
};

#endif