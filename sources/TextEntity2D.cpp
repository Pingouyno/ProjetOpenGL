#include"../headers/TextEntity2D.h"

const string TextEntity2D::DEFAULT_TEXT = "Entrer du texte...";

TextEntity2D::TextEntity2D(vec3 pos, float pixFontSize, string text) : Entity(pos)
{
    this->pixFontSize = pixFontSize;

    borderQuad = new Quad2D(vec3(0, 0, 0), 1, 1, Texture::get2DImgTexture("square.png"));
    borderQuad->active = false;
    addShape(borderQuad);

    setText(text);
}

//détecte les collisions de borderQuad, MÊME s'il n'est pas actif
void TextEntity2D::checkCollisions(vec3 mousePos)
{
    if (borderQuad->isColliding(mousePos))
    {
        if (selectedTextBox != this) 
        {
            this->select();
        }

        else 
        {
            setText(text + "s");
        }
    }
}

/*resize entityShapes, mais ne détruit pas les Quad2D désormais inutiles ; va juste faire active = false dans textQuads. 
 textQuads garde par conséquent la même taille qu'avant*/
void TextEntity2D::setText(string text)
{
    const int totalPixWidth = pixFontSize * text.size(); 
    const int oldSize = createdQuads.size();
    this->text = text;

    if (text.size() > oldSize)
    {
        createdQuads.resize(text.size());
    }

    vec2 offSet(Shape::toXRatio(- totalPixWidth / 2.0f + pixFontSize / 2.0f), - Shape::toYRatio(TextEntity::Y_TO_X_RATIO * pixFontSize) / 2.0f);

    //important de générer le borderQuad avant d'avoir modifié l'offset
    borderQuad->resize(totalPixWidth, TextEntity::Y_TO_X_RATIO * pixFontSize);
    borderQuad->moveTo(getPos() + vec3(offSet, 0));
    
    vec2 pos = vec2(getPos());
    for (int i = 0 ; i < text.size() ; i++)
    {
        if (i < oldSize)
        {
            createdQuads[i]->setTexture(TextManager::getTextTexture(text[i]));
            createdQuads[i]->moveTo(vec3(pos + offSet, 0));
        }else 
        {
            createdQuads[i] = new Quad2D(pos + offSet, pixFontSize, TextEntity::Y_TO_X_RATIO * pixFontSize, TextManager::getTextTexture(text[i]));
        }
        offSet += vec2(Shape::toXRatio(pixFontSize), 0);
    }

    //manipuler les nouveaux vectors
    textQuads = createdQuads;
    textQuads.resize(text.size());
    
    //on garde le borderQuad au début, pour qu'il soit toujours render en premier
    entityShapes.resize(1);
    entityShapes.insert(entityShapes.end(), textQuads.begin(), textQuads.end());
}

void TextEntity2D::select()
{
    this->borderQuad->active = true;
    if (selectedTextBox != nullptr && selectedTextBox != this) ((TextEntity2D*)selectedTextBox)->deselect();
    selectedTextBox = this;
}

void TextEntity2D::deselect()
{
    this->borderQuad->active = false;
   /* if (this->text != DEFAULT_TEXT)*/ setText(DEFAULT_TEXT);
    selectedTextBox = nullptr;
}