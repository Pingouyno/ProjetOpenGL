#include"../headers/TextManager.h"

string TextManager::TEXT_PATH = "resources/textSamples/";
Shader* TextManager::shader = nullptr;
vector<Texture*> TextManager::numbers({});

void TextManager::initTextures()
{
    for (int i = 0 ; i < 10 ; i++)
    {
        string s = (TEXT_PATH + "number_" + to_string(i) + ".png");
        Texture* number_png = new Texture(s.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
        number_png->texUnit(*shader, "tex0", 0);
        numbers.push_back(number_png);
    }
}

void TextManager::bindToShader(Shader *shader)
{
    TextManager::shader = shader;
    initTextures();
}

Texture* TextManager::getNumberTexture(char number)
{
    return numbers[number - '0']; 
}