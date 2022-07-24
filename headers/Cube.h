#ifndef CUBE_CLASS_H
#define CUBE_CLASS_H

#include"Shape.h"
#include<vector>
using namespace std;

class Cube : public Shape
{
    public:
        //définit les coordonnées absolues du mapping texture de cube
        static const vector<float> CUBE_TEXMAP;
        static const int VERTICE_COUNT = 12;   //Taille du tableau 3 fois plus grosse que 8 car un vertex a (x, y, z)
        static const int INDICE_COUNT = 36;    //2 triangles par côté, 6 côtés par cube = 3 * 2 * 6 = 36        

        float size;

        Cube(vector<float> &pos, float &size, Texture* texture);
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