#ifndef TEXTMANAGER_CLASS_H
#define TEXTMANAGER_CLASS_H

#include<string>
#include<vector>
using namespace std;

#include"Texture.h"
#include"shaderClass.h"

//regader : https://seotoolscentre.com/text-to-image-generator

class TextManager
{
    private:
        static string TEXT_PATH;
        static Shader* shader;
        static vector<Texture*> numbers;
    
    public:
        static void initTextures();
        static void bindToShader(Shader* Shader);
        static Texture* getNumberTexture(int number);
        static Texture* getTextTexture(string textFileName);
};
#endif