#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include<string>
using namespace std;

#include"../libraries/include/stb/stb_image.h"
#include"../libraries/include/GLAD/glad.h"

#include"Globals.h"
#include"shaderClass.h"


class Texture {
public:
	const static string TEXTURE_PATH;
	
	GLuint ID;
	GLenum type;
	int widthImg;
	int heightImg;
	Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType);
	Texture(string imgHeadName);

	// Assigns a texture unit to a texture
	void texUnit(Shader& shader, const char* uniform, GLuint unit);
	// Binds a texture
	void Bind();
	// Unbinds a texture
	void Unbind();
	// Deletes a texture
	void Delete();

	static Texture* get2DImgTexture(string textureFileName);
	static Texture* get3DImgTexture(string textureFileName);
};
#endif