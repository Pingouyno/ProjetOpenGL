#ifndef CUBE_CLASS_H
#define CUBE_CLASS_H

#include"Shape.h"
#include<vector>
using namespace std;

class Cube : public Shape
{
    public:
        static const int VERTICE_COUNT = 8;     //Taille du tableau 3 fois plus grosse que 8 car un vertex a (x, y, z)
        static const int INDICE_COUNT = 36;     //2 triangles par côté, 6 côtés par cube = 3 * 2 * 6 = 36
        vector<float> CUBE_VERTICES;            
        vector<int> CUBE_INDICES;                

        float size;

        Cube(vector<float> &pos, float &size, vector<float> &color);
        Cube(vector<float> &pos, float &size);

        void spawn();
        void despawn();
        void moveTo(float x, float y, float z);
        void render();
        bool isColliding(Camera camera);
        void setSize(float size);

    private:
        void initIndices();
        void initVertices();
        void refreshGLVertices();
        void generate();
        
};

#endif