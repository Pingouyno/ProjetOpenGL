#ifndef TEXT_ENTITY_2D_CLASS_H
#define TEXT_ENTITY_2D_CLASS_H

#include"Entity.h"
#include"Quad.h"
#include"Quad2D.h"
#include"TextEntity.h"
#include"TextManager.h"

//Peut seulement avoir des Quad2D, sinon castException sur checkCollisions
class TextEntity2D : public Entity
{
    private:
        //formes du TextEntity
        vector<Quad2D*> textQuads;
        vector<Quad2D*> createdQuads;
        
    public:
        const static string DEFAULT_TEXT;
        //forme qui détecte les collisions MÊME lorsqu'elle n'est pas active, public car on la modifie dans overlay pour déselectionner
        Quad2D* borderQuad;
        float pixFontSize;
        string text;
        TextEntity2D(vec3 pos, float pixFontSize, string text);
        void checkCollisions(vec3 mousePos);
        void setText(string text);
        void select();
        void deselect();
        static void setSelectedTextBox(TextEntity2D* textBox);
};
#endif