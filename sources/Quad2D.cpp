#include"../headers/Quad2D.h"

//pos : ratio ;  pixWidth : pixel
Quad2D::Quad2D(glm::vec2 pos, float pixWidth, float pixHeight, glm::vec3 color, Texture* tex, function <void(void)> clickLogic)
{
    this->clickLogic = clickLogic;
    this->pixWidth = pixWidth;
    this->pixHeight = pixHeight;
    this->pixPosX = Shape::toXPixelCoord(pos[0]);
    this->pixPosY = Shape::toYPixelCoord(pos[1]);
    initQuad(glm::vec3(pos, 0), Shape::toXRatio(pixWidth), Shape::toYRatio(pixHeight), color, tex, OVERLAY);
}

Quad2D::Quad2D(glm::vec2 pos, float pixWidth, float pixHeight, Texture* tex, function <void(void)> clickLogic)
    : Quad2D(pos, pixWidth, pixHeight, DEFAULT_COLOR, tex, clickLogic){}

Quad2D::Quad2D(glm::vec2 pos, float pixWidth, float pixHeight, Texture* tex)
    : Quad2D(pos, pixWidth, pixHeight, DEFAULT_COLOR, tex, [](){}){}

Quad2D::Quad2D(glm::vec2 pos, float pixWidth, float pixheight, vec3 color)
    : Quad2D(pos, pixWidth, pixHeight, color, nullptr, [](){}){}

Quad2D::Quad2D(glm::vec2 pos, Texture* tex)
: Quad2D(pos, tex->widthImg, tex->heightImg, DEFAULT_COLOR, tex, [](){}){}

void Quad2D::resize(float pixWidth, float pixHeight)
{
    Quad::resize(Shape::toXRatio(pixWidth), Shape::toYRatio(pixHeight));
    this->pixWidth = pixWidth;
    this->pixHeight = pixHeight;
}

void Quad2D::moveTo(vec3 pos)
{
    Shape::moveTo(pos);
    this->pixPosX = Shape::toXPixelCoord(pos[0]);
    this->pixPosY = Shape::toYPixelCoord(pos[1]);
}

/*pour la souris, on calcule la position de pixel de la souris.
 position de la souris commence en HAUT À GAUCHE.
 par conséquent, on veut souris qui a x > pos et y < pos, puisque les Quad2D commencent en BAS À GAUCHE*/
bool Quad2D::isColliding(glm::vec3 &mousePos)
{
    float diffX = mousePos[0] - pixPosX;
    float diffY = pixPosY - mousePos[1];
    
    return (diffX >= 0 && diffX < pixWidth 
        && diffY >= 0 && diffY < pixHeight);
}

void Quad2D::doClickLogic()
{
    clickLogic();
}