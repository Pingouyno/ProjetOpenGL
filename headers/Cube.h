#ifndef CUBE_CLASS_H
#define CUBE_CLASS_H

#include"Shape.h"

class Cube : public Shape {
    public:
        //définit les coordonnées absolues du mapping texture de cube
        static int VERTICE_COUNT;
        static int INDICE_COUNT; 
        static vector<float> SHAPE_TEXMAP;  

        float depth;

        void initCube(glm::vec3 pos, float width, float height, float depth, glm::vec3 color, Texture* tex);
        Cube(glm::vec3 pos, glm::vec3 dimensions, Texture* tex);
        Cube(glm::vec3 pos, float width, float height, float depth, glm::vec3 color);
        Cube(glm::vec3 pos, float width, float height, Texture* tex);
        Cube(glm::vec3 pos, float width, float height, glm::vec3 color);
        Cube(glm::vec3 pos, float size, Texture* tex);
        Cube(glm::vec3 pos, float size, glm::vec3 color);
        Cube(glm::vec3 pos, float size);
        
        int getVerticeCount();
        int getIndiceCount();
        vector<float> getShapeTexMap();
        
        void render();
        void resize(float width, float height);
        void resize(float width, float height, float depth);
        bool isColliding(glm::vec3 &camPos);  

    private:
        void initIndices();
        void initVertices();
};

#endif