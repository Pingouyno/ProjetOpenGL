#ifndef SHAPE_CLASS_H
#define SHAPE_CLASS_H

#include<vector>
#include<iterator>
#include<iostream>
using namespace std;

#include"../libraries/include/glm/glm.hpp"

#include"GlobalArrays.h"
#include"Texture.h"


class Shape {
    public:
        //axe sur lequel on peut "enfiler en brochette" le quad ; pour une plateforme horizontale on a 'Y'.
        enum Type : char;

        static glm::vec3 DEFAULT_COLOR;
        static vector<float> DEFAULT_TEXMAP;
        static vector<float> NO_TEXMAP;

        //propriétés hitbox Camera
        static float camBoxHeight;
        static float camBoxWidth;

        static float screenWidth;
        static float screenHeight;

        Shape();

        //variables/fonctions à utiliser et redéfinir;

        glm::vec3 pos;
        glm::vec3 color;
        vector<float> texMap;
        vector<float> shapeVertices;  //non statique car change           
        vector<int> shapeIndices;    
        bool active = false;
        bool isRenderingColor;
        int indexInVertices = -1;
        int indexInIndices = -1;
        Texture* tex = nullptr;

        virtual void render();
        virtual void resize(float &size); 
        virtual bool isColliding(glm::vec3 &camPos);
        virtual int getVerticeCount();
        virtual int getIndiceCount();
        virtual vector<float> getShapeTexMap();

    protected:
        //inscrit dans le log si les déplacements individuels de x, y et z de camera vers newPos mènent à une collision
        virtual void initIndices();
        virtual void initVertices();
        
        //**fin fonctions à redéfinir**


    //fonctions reliées à Shape, ne PAS redéfinir
    public:
        static void initVariables(float screenWidth, float screenHeight);
        static float toXRatio(float pixSize);
        static float toYRatio(float pixSize);
        static float toXPixelCoord(float xRatio);
        static float toYPixelCoord(float yRatio);

        void spawn();
        void despawn();
        void moveTo(float &x, float &y, float &z);
        void moveTo(float (&pos)[3]);
        bool hasTexture();
        void reportCollision(vector<int> &collisionLog, glm::vec3 &oldPos, glm::vec3 &newPos);

        //pour la redéfinition de variables statiques
        void refreshGLVertices();
        void generate();

    private:
        void printUndefinedErr(string funcName);
};

#endif