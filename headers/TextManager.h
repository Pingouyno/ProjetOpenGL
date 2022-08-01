#ifndef TEXTMANAGER_CLASS_H
#define TEXTMANAGER_CLASS_H

#include<string>
#include<vector>
#include"Texture.h"
using namespace std;


class TextManager
{
    private:
        static string TEXT_PATH;
        static Shader* shader;
        static vector<Texture*> numbers;
    
    public:
        static void initTextures();
        static void bindToShader(Shader* Shader);
        static Texture* getNumberTexture(char number);
};



#endif