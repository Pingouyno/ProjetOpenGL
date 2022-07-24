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

        //true si on a créé une nouvelle entité depuis le dernier rendering
        static bool newShapeCreated;


        //variables à utiliser et redéfinir 
        vector<float> pos;
        vector<float> color;
        vector<float> texMap;
        vector<float> shapeVertices;            
        vector<int> shapeIndices;    
        bool active = false;
        bool isRenderingColor;
        int indexInVertices = -1;
        int indexInIndices = -1;
        Texture* tex = nullptr;

        Shape(){
            newShapeCreated = true;
        }

        virtual void spawn();

        virtual void despawn();

        virtual void moveTo(float x, float y, float z);

        const void moveTo(float (&pos)[3]);

        virtual void render();

        virtual bool isColliding(Camera Camera);

        virtual void setSize(float size);

        bool hasTexture();

        static void renderActiveShapes();

        static Shape* checkCameraCollidingAnyShape(Camera Camera);

    protected:
        void initIndices();
        void initVertices();
        void refreshGLVertices();
        void generate();

};

#endif