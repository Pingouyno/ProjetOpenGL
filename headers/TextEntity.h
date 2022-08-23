#ifndef TEXT_ENTITY_CLASS_H
#define TEXT_ENTITY_CLASS_H

#include"Entity.h"
#include"Quad.h"
#include"TextManager.h"

//Fonctionne seulement pour axe Z
class TextEntity : public Entity
{
    public:
        const static Quad::Axis DEFAULT_AXIS;
        const static float Y_TO_X_RATIO;

        Quad::Axis textAxis;
        string text;
        TextEntity(vec3 pos, float width, string text);
};
#endif