#include<iostream>
#include<unistd.h>
#include<cmath>
#include<vector>
#include<thread>
using namespace std;
#include<chrono>
using namespace std::chrono;
#include"libraries/include/GLAD/glad.h"
#include"libraries/include/glfw/glfw3.h"
#include"libraries/include/stb/stb_image.h"
#include"libraries/include/glm/glm.hpp"
#include"libraries/include/glm/gtc/matrix_transform.hpp"
#include"libraries/include/glm/gtc/type_ptr.hpp"
#include"libraries/include/IrrKlang/include/irrKlang.h"
using namespace irrklang;

#include"headers/shaderClass.h"
#include"headers/VAO.h" 
#include"headers/VBO.h"
#include"headers/EBO.h"
#include"headers/Texture.h"
#include"headers/Camera.h"
#include"headers/GlobalArrays.h"
#include"headers/Cube.h"
#include"headers/Quad.h"

const int width = 1440;//1420;
const int heigth = 900;//800;
const int FRAME_MILLI = 17;

//MÉTHODES UTILISÉES____________________________________________________________

void checkProgramKill(GLFWwindow *window);
void doRenderLogic(GLFWwindow* window, int seed);
void reloadVerticesInVBO(VBO &VBO1);
void reloadIndicesInEBO(EBO &EBO1);
void setup3DWorld(Shader &shaderProgram3D, Camera &camera);
void setup2DOverlay(Shader &shaderProgram2D, Camera &camera);
void draw2DVertices();
void draw3DVertices();

//______________________________________________________________________________

int main()
{	
	//démarrer l'engin irrKlang pour le son
	ISoundEngine* soundEngine = createIrrKlangDevice();

	//vérifier que l'engin a pu démarrer, donc ptr != nullptr
	if (!soundEngine)
	{
		printf("Could not startup engine\n");
		return 0;
	}

	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	GLFWwindow* window = glfwCreateWindow(width, heigth, "ProjetOpenGL", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, width, heigth);


	// Create Vertex Shader Object and get its reference
	Shader shaderProgram3D("resources/shaders/default.vert", "resources/shaders/default.frag"); 
	
	Shader shaderProgram2D("resources/shaders/default.vert2D", "resources/shaders/default.frag"); 

	// Create reference containers for the Vartex Array Object and the Vertex Buffer Object
	VAO VAO1;
	VAO1.Bind();

	VBO VBO1(&vertices[0], vertices.size()* sizeof(float));

	EBO EBO1(&indices[0], indices.size() * sizeof(float));

	//void LinkAttrib(VBO VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);

	//On lie la première moitié de l'ensemble de 8 composants aux coordonnées (x, y, z). Donc vec0 et offset = 0
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8*sizeof(float), (void*)0);

	//On lie la partie des couleurs, soit la deuxième moitié des 8 composants (r, g, b) Donc vec1 et offset = 3 * (taille de float en bits)
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8*sizeof(float), (void*)(3 * sizeof(float)));

	//On lie la partie du mapping texture, qui correspond aux 2 derniers des 8 composantes (x, y)
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8*sizeof(float), (void*)(6 * sizeof(float)));

	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	Camera camera(width, heigth, glm::vec3(0.0f, 0.0f, 0.2f));

	setup3DWorld(shaderProgram3D, camera);
	setup2DOverlay(shaderProgram2D, camera);

	EBO1.Bind();

	auto targetTime = high_resolution_clock::now();
	targetTime += milliseconds(FRAME_MILLI);

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
		shaderProgram3D.Activate();
		
		camera.Inputs(window, soundEngine);
		camera.Matrix(45.0f, 0.01f, 200.0f, shaderProgram3D, "camMatrix");

		// Bind the VAO so OpenGL knows to use it
		VAO1.Bind();
		 
		reloadVerticesInVBO(VBO1);
		reloadIndicesInEBO(EBO1);

		draw3DVertices();

		shaderProgram2D.Activate();
		draw2DVertices();

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);

		// Take care of all GLFW events
		glfwPollEvents();

		checkProgramKill(window);

		std::this_thread::sleep_until(targetTime);
		targetTime = high_resolution_clock::now();
		targetTime += milliseconds(FRAME_MILLI);
	}


	// Delete all the objects we've created

	Shape::deleteAllShapes();
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	//deux_png.Delete();
	//flag_png.Delete();
	//grass_png.Delete();
	shaderProgram3D.Delete();
	shaderProgram2D.Delete();

	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();

	//arrêter l'engin de son
	soundEngine->drop();

	return 0;
}


void checkProgramKill(GLFWwindow* window){
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS 
			&& glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}


void reloadVerticesInVBO(VBO &VBO1){
	//Mettre à jour le tableau et re-charger le VBO. On prend SubData pour optimisation
	VBO1.Bind();
	
	if (Shape::shouldReloadArrays)
	{
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);
	}else
	{
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), &vertices[0]);
	}
	
}

void reloadIndicesInEBO(EBO &EBO1){
	EBO1.Bind();
	if (Shape::shouldReloadArrays)
	{
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), &indices[0], GL_DYNAMIC_DRAW);
	}else
	{
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(float), &indices[0]);
	}

}

void setup3DWorld(Shader &shaderProgram3D, Camera &camera)
{
	Texture* deux_png = new Texture("resources/textures/deux_icon.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	(*deux_png).texUnit(shaderProgram3D, "tex0", 0);

	Texture* grass_png = new Texture("resources/textures/grass.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	(*grass_png).texUnit(shaderProgram3D, "tex0", 0);

	vector<float> pos({-10.0f, -10.0f, -10.0f});
	float wallSize = 2.5f;
	int x = 0;

	camera.Position = glm::vec3(pos[0]+wallSize/2, pos[1] + wallSize/1.5, pos[2]+wallSize/2);
	camera.Orientation = glm::rotate(camera.Orientation, glm::radians(240.0f), glm::vec3(0, 1.0f, 0));

	//générer les murs du labyrinthe
	for (int z = 0 ; z < LAB_SIZE + 1; z++)
	{
		for (int x = 0 ; x < LAB_SIZE + 1 ; x++)
		{
			if (z != LAB_SIZE)
			{
				if (x != LAB_SIZE)
					Shape::shapes.push_back(new Quad(pos, wallSize, deux_png, Quad::Axis::Y));

				if (x == 0 || (x == LAB_SIZE && z != LAB_SIZE - 1)) 
					Shape::shapes.push_back(new Quad(pos, wallSize, grass_png, Quad::Axis::X));
			}

			if (z % LAB_SIZE == 0 && x != LAB_SIZE) 
				Shape::shapes.push_back(new Quad(pos, wallSize, grass_png, Quad::Axis::Z));

			pos[0] += wallSize;

		}

		pos[0] -= (LAB_SIZE + 1) * wallSize;
		pos[2] += wallSize;
	}

	pos = {-10.0f, -10.0f, -10.0f};
	int cpt = 0;
	for (int i = 0 ; i < labyrinth.size(); i++)
	{

		if (labyrinth[i] == 1)
			Shape::shapes.push_back(new Cube(pos, wallSize, grass_png));
		pos[0] += wallSize;

		cpt++;
		if (cpt == LAB_SIZE)
		{
			cpt = 0;
			pos[0] -= LAB_SIZE * wallSize;
			pos[2] += wallSize;
		}
	}
}

void setup2DOverlay(Shader &shaderProgram2D, Camera &camera)
{

	//Pour blend les endroits vides des png
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Texture* crosshair_png = new Texture("resources/textures/crosshair.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	(*crosshair_png).texUnit(shaderProgram2D, "tex0", 0);

	vector<float> pos({-0.03125, -0.03125, 0.0f});
	float size = 0.0625f;

	Shape::shapes2D.push_back(new Quad(pos, size, crosshair_png, Quad::Axis::Z));
}

void draw3DVertices()
{
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	Shape::renderActive3DShapes();
}

void draw2DVertices()
{
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	Shape::renderActive2DShapes();
}
