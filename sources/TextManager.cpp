#include"../headers/TextManager.h"

const string TextManager::CHARACTERS = "abcdefghijklmnopqrstuvwxyzABCEDFGHIJKLMNOPQRSTUVWXYZ0123456789:._-()";
string TextManager::TEXT_PATH = "resources/textSamples/";
vector<Texture*> TextManager::textures(256);
Texture* TextManager::WHITESPACE_TEXTURE = nullptr;

void TextManager::initTextures()
{
    TextManager::WHITESPACE_TEXTURE = TextManager::getTextTexture("whitespace");

    for (char character : CHARACTERS)
    {
        string s = (TEXT_PATH + "text_" + character + ".png");
        Texture* png = new Texture(s.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
        png->texUnit(*shaderProgram2D, "tex0", 0);
        textures[character] = png;
    }
}

void TextManager::init()
{
    initTextures();
}

Texture* TextManager::getNumberTexture(int number)
{
    Texture* tex = textures[number + '0'];
    return tex; 
}

Texture* TextManager::getTextTexture(string textFileName)
{
    if (textFileName.length() == 1) return getTextTexture(textFileName[0]);
    string s = (TEXT_PATH + "text_" + textFileName + ".png");
    Texture* text_png = new Texture(s.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    text_png->texUnit(*shaderProgram2D, "tex0", 0);
    return text_png;
}

Texture* TextManager::getTextTexture(char character)
{
    if (character == ' ') return WHITESPACE_TEXTURE;
    else if (textures[character] == nullptr)
    {
        cout << "\n\n**ERREUR : Texture de charactère \" " << character << " \" manquant**\n\n";
        throw 1;
    }
    return textures[character];
}