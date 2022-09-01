#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include<string>
#include<map>
using namespace std;

#include"../libraries/include/stb/stb_image.h"
#include"../libraries/include/GLAD/glad.h"

#include"Globals.h"
#include"shaderClass.h"

/*NOTE : on ne peut pas utiliser les mêmes images de texture à la fois pour le 3D et le 2D, 
  car openGL garde en mémoire et prend les données de la première fois que l'image a été chargée,
  or ils utilisent des shaders différents et le nombre de color channels va varier.
  ne JAMAIS importer des photos de cubemap vers textures ou vice-versa*/
class Texture {
	public:

		//**important : modifier les noms dans Texture.cpp si l'on ajout des textures**
		enum TEX3D : int 
		{
			FIELD = 0,
			GRASS = 1,
			AIR = 2,
			COORDINATES = 3,
			CITY = 4,
			DIRT = 5,
			STONE = 6,
			BEDROCK = 7,
			STEVE_HEAD = 8,
			STEVE_BODY = 9,
			STEVE_RIGHT_LEG = 10,
			STEVE_LEFT_LEG = 11,
			STEVE_RIGHT_ARM = 12,
			STEVE_LEFT_ARM = 13,
			SKYBOX = 14,
			LEAVES = 15,
			WOOD = 16,
		};

		//BLOCS
		static Texture* Air;
	    static Texture* Dirt;
		static Texture* Grass;
		static Texture* Stone;
	    static Texture* Bedrock;
		static Texture* Leaves;
		static Texture* Wood;

		const static vector<string> tex3DNames;
		static vector<Texture*> textures3D;

		const static string TEXTURE_PATH;
		const static string CUBEMAP_PATH;
		
		TEX3D tex3Did;
		GLuint ID;
		GLenum type;
		int widthImg;
		int heightImg;
		Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType);
		Texture(string imgHeadName);
		//pour un cube 3D
		Texture(string cubeMapHeadName, GLenum texType, TEX3D tex3Did);

		// Assigns a texture unit to a texture
		void texUnit(Shader& shader, const char* uniform, GLuint unit);
		// Binds a texture
		void Bind();
		// Unbinds a texture
		void Unbind();
		// Deletes a texture
		void Delete();

		static Texture* get2DImgTexture(string textureFileName);
		static Texture* get3DImgTexture(TEX3D textureName);

		static void init3DTextures();

	private:
		static Texture* load3DImgTexture(TEX3D tex3Did);
		static vector<string> getTexPathsFromFileName(string cubeMapHeadName);	
};
#endif