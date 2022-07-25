#ifndef CUBE_CLASS_H
#define CUBE_CLASS_H

#include"Shape.h"
#include<vector>
using namespace std;

class Cube : public Shape
{
    public:
        //définit les coordonnées absolues du mapping texture de cube
        static int VERTICE_COUNT;
        static int INDICE_COUNT; 
        static vector<float> SHAPE_TEXMAP;  
        float size;

        void initCube(vector<float> &pos, float &size, vector<float> &color, Texture* tex);
        Cube(vector<float> &pos, float &size, Texture* tex);
        Cube(vector<float> &pos, float &size, vector<float> &color);
        Cube(vector<float> &pos, float &size);
        
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