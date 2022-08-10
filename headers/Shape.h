#ifndef SHAPE_CLASS_H
#define SHAPE_CLASS_H

#include<vector>
#include<iterator>
#include<iostream>
#include<cmath>
using namespace std;

#include"../libraries/include/glm/glm.hpp"
#include"../libraries/include/glm/gtc/matrix_transform.hpp"
#include"../libraries/include/glm/gtc/type_ptr.hpp"
#include"../libraries/include/glm/gtx/rotate_vector.hpp"
#include"../libraries/include/glm/gtx/vector_angle.hpp"
#include"../libraries/include/glm/gtx/string_cast.hpp"
using namespace glm;

#include"Globals.h"
#include"Texture.h"

//Le formes vont du SUD-OUEST (-x, -y) au NORD-OUEST (+x, +y) et du BAS vers le HAUT
class Shape {
    private:
        //rotation verticale = x, horizontale = y, de côté = z
        glm::vec3 dirVecX;
        glm::vec3 dirVecY;
        glm::vec3 dirVecZ;

    public:
        //axe sur lequel on peut "enfiler en brochette" le quad ; pour une plateforme horizontale on a 'Y'.
        enum Type : char;

        static glm::vec3 DEFAULT_COLOR;
        static vector<float> DEFAULT_TEXMAP;
        static vector<float> NO_TEXMAP;

        const static vec3 ROT_X;
        const static vec3 ROT_Y;
        const static vec3 ROT_Z;

        const static vec4 AXIS_X;
        const static vec4 AXIS_Y;
        const static vec4 AXIS_Z;

        //propriétés hitbox Camera
        static float camBoxHeight;
        static float camBoxWidth;

        Shape();

        //variables/fonctions à utiliser et redéfinir;

        //rotation verticale = x, horizontale = y, de côté = z
        mat4 originTransposition;
        glm::vec3 pos;
        glm::vec3 color;
        float width;
        float height;
        vector<float> texMap;
        vector<float> shapeVertices;  //non statique car change           
        vector<int> shapeIndices;    
        bool active = false;
        bool isRenderingColor;
        int indexInVertices = -1;
        int indexInIndices = -1;
        Texture* tex = nullptr;

        virtual void render();
        virtual void resize(float width, float height);
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
        static float getAngleRadians(vec2 a, vec2 b);
        static float getAngleRadians(vec3 a, vec3 b);
        static vec4 getVecIfRotate(vec3 axis, float radians, vec3 vectorToRotate);

        void spawn();
        void despawn();
        void moveTo(float x, float y, float z);
        void moveTo(glm::vec3 pos);
        void resize(float size);
        void rotate(vec3 axis, float radians);
        void rotateAround(vec3 pos, vec3 axis, float radians);
        void lookAt(glm::vec3 targetPos);
        void lookAtHorizontal(glm::vec3 targetPos);
        void lookAtVertical(glm::vec3 targetPos);
        void resetRotation();
        bool hasTexture();
        void reportCollision(vector<int> &collisionLog, glm::vec3 &oldPos, glm::vec3 &newPos);
        vec3 getXAxis();
        vec3 getYAxis();
        vec3 getZAxis();

        //pour la redéfinition de variables statiques
        void refreshGLVertices();
        void generate();

    private:
        float degreesToRadians(float degrees);
        float radiansToDegrees(float radians);
        void printUndefinedErr(string funcName);
};

#endif