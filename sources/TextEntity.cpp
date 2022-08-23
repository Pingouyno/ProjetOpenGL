#include"../headers/TextEntity.h"

const Quad::Axis TextEntity::DEFAULT_AXIS = Quad::Axis::Z;
const float TextEntity::Y_TO_X_RATIO = 48.0f/24.0f;

TextEntity::TextEntity(vec3 pos, float width, string text) : Entity(pos)
{
    this->text = text;
    this->textAxis = DEFAULT_AXIS;
    float charWidth = width / text.length();

    vec3 offSet(- width / 2.0f + charWidth / 2.0f, 0, 0);
    for (char letter : text)
    {
        Texture* tex = TextManager::getTextTexture(letter);
        addShape(new Quad(pos + offSet, tex, charWidth, textAxis));
        offSet += vec3(charWidth, 0, 0);
    }
    
}