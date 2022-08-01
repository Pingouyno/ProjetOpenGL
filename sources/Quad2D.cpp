#include"../headers/Quad2D.h"


vector<Quad2D*> Quad2D::shapesHUDCollidable({});

//pos : ratio ;  pixWidth : pixel
Quad2D::Quad2D(vector<float> &pos, float pixWidth, float pixHeight, Texture* tex, function <void(void)> clickLogic)
{
    this->clickLogic = clickLogic;
    this->pixWidth = pixWidth;
    this->pixHeight = pixHeight;
    this->pixPosX = Shape::toXPixelCoord(pos[0]);
    this->pixPosY = Shape::toYPixelCoord(pos[1]);
    initQuad(pos, Shape::toXRatio(pixWidth), Shape::toYRatio(pixHeight), DEFAULT_COLOR, tex, AXIS_2D);
}

Quad2D::Quad2D(vector<float> &pos, float pixWidth, float pixHeight, Texture* tex)
    : Quad2D(pos, pixWidth, pixHeight, tex, [](){}){}


//pour la souris, on calcule la position de pixel de la souris.
//position de la souris commence en HAUT À GAUCHE.
bool Quad2D::isColliding(glm::vec3 &mousePos)
{
    //cout << "\n" << pixPosX << "/" << pixPosX + pixWidth  << " | " << pixPosY - pixHeight<< "/" << pixPosY;
    //cout << "--> " <<mousePos[0] << " " << mousePos[1] << " " << mousePos[2] << " \n";
    return (mousePos[0] > pixPosX && mousePos[0] < pixPosX + pixWidth
        && mousePos[1] > pixPosY - pixHeight && mousePos[1] < pixPosY);
}

void Quad2D::doClickLogic()
{
    clickLogic();
}