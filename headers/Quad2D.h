#ifndef QUAD2D_CLASS_H
#define QUAD2D_CLASS_H

#include<functional> 
using namespace std;

#include"Quad.h"

class Quad2D : public Quad {

    float pixWidth;
    float pixHeight;
    float pixPosX;
    float pixPosY;

    public:
        static vector<Quad2D*> shapesHUDCollidable;
        Quad::Axis AXIS_2D = Axis::Z;

    //potentiellement remettre public
    public:
        function <void(void)> clickLogic;

    public:
        Quad2D(glm::vec2 pos, float pixWidth, float pixheight, Texture* tex, function <void(void)> clickLogic);
        Quad2D(glm::vec2 pos, float pixWidth, float pixheight, Texture* tex);

        bool isColliding(glm::vec3 &mousePos);
        
        void doClickLogic();
};

#endif