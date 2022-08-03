#ifndef CUBE_CLASS_H
#define CUBE_CLASS_H

#include"Shape.h"

class Cube : public Shape {
    public:
        //définit les coordonnées absolues du mapping texture de cube
        static int VERTICE_COUNT;
        static int INDICE_COUNT; 
        static vector<float> SHAPE_TEXMAP;  
        float size;

        void initCube(glm::vec3 pos, float size, glm::vec3 color, Texture* tex);
        Cube(glm::vec3 pos, float size, Texture* tex);
        Cube(glm::vec3 pos, float size, glm::vec3 color);
        Cube(glm::vec3 pos, float size);
        
        int getVerticeCount();
        int getIndiceCount();
        vector<float> getShapeTexMap();
        
        void render();
        void resize(float &size); 
        bool isColliding(glm::vec3 &camPos);  

    private:
        void initIndices();
        void initVertices();
};

#endif