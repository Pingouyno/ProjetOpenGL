#ifndef QUAD2D_CLASS_H
#define QUAD2D_CLASS_H

#include<functional> 
using namespace std;

#include"Quad.h"

//Les coordonnées d'un quad2D commencent en bas à gauche, plutôt qu'au centre de gravité de la forme
class Quad2D : public Quad {

    private:
        float pixWidth;
        float pixHeight;
        float pixPosX;
        float pixPosY;
        Quad2D(glm::vec2 pos, float pixWidth, float pixheight, vec3 color, Texture* tex, function <void(void)> clickLogic);

    public:
        function <void(void)> clickLogic;
        Quad2D(glm::vec2 pos, float pixWidth, float pixheight, Texture* tex, function <void(void)> clickLogic);
        Quad2D(glm::vec2 pos, float pixWidth, float pixheight, Texture* tex);
        Quad2D(glm::vec2 pos, float pixWidth, float pixheight, vec3 color);
        Quad2D(glm::vec2 pos, Texture* tex);

        //redéfinitions
        void resize(float pixWidth, float pixHeight);
        void moveTo(vec3 pos);
        bool isColliding(glm::vec3 &mousePos);
        
        void doClickLogic();

};

#endif