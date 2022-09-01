#include"../headers/Texture.h"

const vector<string> Texture::tex3DNames = 
{
	"field",
	"grass",
	"earth",
	"coordinates",
	"city",
	"dirt",
	"stone",
	"bedrock",
	"stevehead",
	"stevebody",
	"steverightleg",
	"steveleftleg",
	"steverightarm",
	"steveleftarm",
	"skybox",
	"leaves",
	"wood"
};

Texture* Texture::Air = nullptr;
Texture* Texture::Dirt = nullptr;
Texture* Texture::Grass = nullptr;
Texture* Texture::Stone = nullptr;
Texture* Texture::Bedrock = nullptr;
Texture* Texture::Leaves = nullptr;
Texture* Texture::Wood = nullptr;

vector<Texture*> Texture::textures3D = {};
const string Texture::CUBEMAP_PATH = "resources/textures/cubemaps/";
const string Texture::TEXTURE_PATH = "resources/textures/";

Texture::Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType)
{
	// Assigns the type of the texture ot the texture object
	type = texType;

	// Stores the width, height, and the number of color channels of the image
	int widthImg, heightImg, numColCh;
	// Flips the image so it appears right side up
	stbi_set_flip_vertically_on_load(true);
	// Reads the image from a file and stores it in bytes
	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);

	// Generates an OpenGL texture object
	glGenTextures(1, &ID);
	// Assigns the texture to a Texture Unit
	glActiveTexture(slot);
	glBindTexture(texType, ID);

	// Configures the type of algorithm that is used to make the image smaller or bigger
	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Configures the way the texture repeats (if it does at all)
	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Extra lines in case you choose to use GL_CLAMP_TO_BORDER
	// float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);

	// Assigns the image to the OpenGL Texture object
	// Check what type of color channels the texture has and load it accordingly

	if (numColCh == 4)
		glTexImage2D
		(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			widthImg,
			heightImg,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			bytes
		);
	else if (numColCh == 3)
		glTexImage2D
		(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			widthImg,
			heightImg,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			bytes
		);
	else if (numColCh == 1)
		glTexImage2D
		(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			widthImg,
			heightImg,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			bytes
		);
	else
	{
		cout << "\n\n**ERREUR 2D : La reconnaissance du type de texture a échoué!**\n\n";
		glTexImage2D(texType, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType, bytes);
	}
		

	// Generates MipMaps
	glGenerateMipmap(texType);

	// Deletes the image data as it is already in the OpenGL Texture object
	stbi_image_free(bytes);

	// Unbinds the OpenGL Texture object so that it can't accidentally be modified
	glBindTexture(texType, 0);

	this->widthImg = widthImg;
	this->heightImg = heightImg;
}

Texture::Texture(string imgHeadName) : Texture(imgHeadName.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE){}


Texture::Texture(string cubeMapHeadName, GLenum texType, TEX3D tex3Did)
{
	this->tex3Did = tex3Did;
	vector<string> texPaths = getTexPathsFromFileName(cubeMapHeadName);

	// Assigns the type of the texture ot the texture object
	type = texType;
	// Generates an OpenGL texture object
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &ID);
	glBindTexture(texType, ID);


	int width, height, numColCh;
	for (unsigned int i = 0; i < 6; i++)
	{
		//**ATTENTION : ICI ON A MIS LE NOMBRE &numColCh À 3** __________________________
		//															  					 V
		unsigned char* data = stbi_load(texPaths[i].c_str(), &width, &height, &numColCh, 3);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);

			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture: " << texPaths[i] << std::endl;
			stbi_image_free(data);
		}
	}

	// Configures the way the texture repeats (if it does at all)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// These are very important to prevent seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Extra lines in case you choose to use GL_CLAMP_TO_BORDER
	// float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);
	// Generates MipMaps
	//glGenerateMipmap(texType);

	//assurer que l'on ne puisse pas re-modifier la texture
	glBindTexture(texType, 0);

	this->widthImg = width;
	this->heightImg = height;
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
	// Gets the location of the uniform
	GLuint texUni = glGetUniformLocation(shader.ID, uniform);
	// Shader needs to be activated before changing the value of a uniform
	shader.Activate();
	// Sets the value of the uniform
	glUniform1i(texUni, unit);
}

void Texture::Bind()
{
	glBindTexture(type, ID);
}

void Texture::Unbind()
{
	glBindTexture(type, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}

//permet de loader une texture 2D sans avoir à la binder à la ligne suivante
Texture* Texture::get2DImgTexture(string textureFileName)
{
    Texture* texture = new Texture(TEXTURE_PATH + textureFileName);
    texture->texUnit(*shaderProgram2D, "tex0", GL_TEXTURE0);
    return texture;
}

//permet de loader une texture 3D sans avoir à la binder à la ligne suivante
Texture* Texture::get3DImgTexture(TEX3D textureName)
{
    return textures3D[textureName];
}

void Texture::init3DTextures()
{
	for (int tex3Did = 0 ; tex3Did < tex3DNames.size() ; tex3Did++)
	{
		textures3D.push_back(load3DImgTexture((TEX3D)tex3Did));
	}

	Air = Texture::get3DImgTexture(Texture::TEX3D::AIR);
	Dirt = Texture::get3DImgTexture(Texture::TEX3D::DIRT);
	Grass = Texture::get3DImgTexture(Texture::TEX3D::GRASS);
	Stone = Texture::get3DImgTexture(Texture::TEX3D::STONE);
	Bedrock = Texture::get3DImgTexture(Texture::TEX3D::BEDROCK);
	Leaves = Texture::get3DImgTexture(Texture::TEX3D::LEAVES);
	Wood = Texture::get3DImgTexture(Texture::TEX3D::WOOD);
}

//permet de loader une texture 3D sans avoir à la binder à la ligne suivante
Texture* Texture::load3DImgTexture(TEX3D tex3Did)
{
	Texture* texture = new Texture(CUBEMAP_PATH + tex3DNames[tex3Did], GL_TEXTURE_CUBE_MAP, tex3Did);
    //mettre les données de texture dans SampleCube
	texture->texUnit(*shaderProgramCube, "cube", GL_TEXTURE0);
	return texture;
}

vector<string> Texture::getTexPathsFromFileName(string cubeMapHeadName)
{
	//teste si l'on réussit à charge le fichier, pour ainsi trouver le nom de l'extension
	int i;
	unsigned char* data = stbi_load((cubeMapHeadName + "/posx.jpg").c_str(), &i, &i, &i, 3);

	string fileType = data? "jpg" : "png";
	vector<string> texPaths;

	if (fileType == "jpg")
	{
		texPaths = 
		{
		cubeMapHeadName + "/posx.jpg",  //right
		cubeMapHeadName + "/negx.jpg",  //left 
		cubeMapHeadName + "/posy.jpg",  //top
		cubeMapHeadName + "/negy.jpg",  //bottom
		cubeMapHeadName + "/posz.jpg",  //front   front = +z et back = -z selon convention openGL (right-handed)
		cubeMapHeadName + "/negz.jpg"   //back 
    	};
	}else
	{
		texPaths = 
		{
		cubeMapHeadName + "/px.png",  //right
		cubeMapHeadName + "/nx.png",  //left 
		cubeMapHeadName + "/py.png",  //top
		cubeMapHeadName + "/ny.png",  //bottom
		cubeMapHeadName + "/pz.png",  //front   front = +z et back = -z selon convention openGL (right-handed)
		cubeMapHeadName + "/nz.png"   //back 
    	};
	}
	return texPaths;
}