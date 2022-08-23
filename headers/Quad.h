#ifndef QUAD_CLASS_H
#define QUAD_CLASS_H

#include"Shape.h"

//Un Quad doit être en axe Z pour utiliser lookAt(), car cette fonction fait pointer l'axe Z sur la position
class Quad: public Shape {
    public:
        static int VERTICE_COUNT;
        static int INDICE_COUNT; 
        static vector<float> SHAPE_TEXMAP;

        //axe sur lequel on peut "enfiler en brochette" le quad ; pour une plateforme horizontale on a 'Y'.
        enum Axis : char { X = 'x', Y = 'y', Z = 'z', OVERLAY = 'o'};

        Axis axis;

        void initQuad(glm::vec3 pos, float width, float height, glm::vec3 color, Texture* tex, Axis axis);
        Quad(vec3 pos, Texture* tex, float scale, Axis axis);
        Quad(glm::vec3 pos, float width, float height, Texture* tex, Axis axis);
        Quad(glm::vec3 pos, float size, float width, glm::vec3 color, Axis axis);
        Quad(glm::vec3 pos, float size, Texture* texture, Axis axis);
        Quad(glm::vec3 pos, float size, glm::vec3 color, Axis axis);
        Quad(glm::vec3 pos, float size, Axis axis);
        Quad(){}

        int getVerticeCount();
        int getIndiceCount();
        vector<float> getShapeTexMap();

        void render();
        void resize(float width, float height);
        void setAxis(Axis axis);
        bool isColliding(glm::vec3 &camPos);
    
    private:
        void initIndices();
        void initVertices();
};

#endif