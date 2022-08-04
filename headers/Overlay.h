#ifndef OVERLAY_CLASS_H
#define OVERLAY_CLASS_H

#include<vector>
using namespace std;

#include"TextManager.h"
#include"Quad2D.h"
#include"Camera.h"
#include"Globals.h"

class Overlay
{
    private:
        vector<Quad2D*> staticShapes;
        vector<Quad2D*> clickableShapes;

    public:
        Camera* camera;
        Overlay(Camera* camera);
        void render();
        void checkCollisions(glm::vec3 &mousePos);
        void addStaticShape(Quad2D* shape);
        void addClickShape(Quad2D* shape);

    //fonctions virtuelles (À ÊTRE REDÉFINIES)
    protected:
        virtual void setupOverlay();

};
#endif