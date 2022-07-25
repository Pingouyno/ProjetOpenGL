#ifndef SHAPE_CLASS_H
#define SHAPE_CLASS_H

#include<vector>
using namespace std;
#include<iterator>
#include<iostream>
#include"Camera.h"
#include"Texture.h"

class Shape{
    public:

        static vector<Shape*> shapes;
        static vector<float> DEFAULT_COLOR;
        static vector<float> DEFAULT_TEXMAP;
        static vector<float> NO_TEXMAP;

        //propriétés hitbox Camera
        static float camBoxHeight;
        static float camBoxWidth;

        //true si on a créé une nouvelle entité depuis le dernier rendering. On doit donc RELOAD plutôt que SUB-RELOAD
        static bool shouldReloadArrays;

        Shape(){
            shouldReloadArrays = true;
        }

        //variables/fonctions à utiliser et redéfinir;

        vector<float> pos;
        vector<float> color;
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
        static void renderActiveShapes();
        static vector<int> checkCameraCollidingAnyShape(glm::vec3 &oldPos, glm::vec3 &newPos);
        static bool isAnyColliding(vector<int> &collisionLog);
        static void addShape(Shape* shape);
        static void deleteAllShapes();

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
        void printUndefinedErr(string funcName)
        {
            cout << "\n\n**ERREUR : fonction <" << funcName << "> non redéfinie hors de Shape**\n\n";
        }
};

#endif