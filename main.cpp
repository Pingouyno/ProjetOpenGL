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

#include"headers/shaderClass.h"
#include"headers/VAO.h" 
#include"headers/VBO.h"
#include"headers/EBO.h"
#include"headers/Texture.h"
#include"headers/Camera.h"
#include"headers/Globals.h"
#include"headers/Cube.h"
#include"headers/Quad2D.h"
#include"headers/PlaySound.h"
#include"headers/TextManager.h"
#include"headers/World.h"
#include"headers/EventManager.h"

const int FRAME_MILLI = 17;

//MÉTHODES UTILISÉES____________________________________________________________

void checkProgramKill(GLFWwindow *window);
void reloadVerticesInVBO(VBO &VBO1);
void reloadIndicesInEBO(EBO &EBO1);
//______________________________________________________________________________

int main()
{	
	PlaySound::startEngine();

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

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "ProjetOpenGL", NULL, NULL);
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
	glViewport(0, 0, screenWidth, screenHeight);

	// Créer le pipeline de shaders et les stocker globalement
	shaderProgram2D = new Shader("resources/shaders/default.vert2D", "resources/shaders/default.frag"); 
    shaderProgram3D = new Shader("resources/shaders/default.vert", "resources/shaders/default.frag"); 

	//pour que les images de texte soient liées au shader 2D
	TextManager::bindToShader(shaderProgram2D);

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

	World* world = new World();
	EventManager* eventManager = new EventManager(world);

	//cacher le curseur
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

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
		shaderProgram3D->Activate();
		
		eventManager->Inputs(window);
		world->camera->Matrix(45.0f, 0.01f, 200.0f, *shaderProgram3D, "camMatrix");

		// Bind the VAO so OpenGL knows to use it
		VAO1.Bind();
		 
		reloadVerticesInVBO(VBO1);
		reloadIndicesInEBO(EBO1);

		world->doEntityBehaviors();
		world->render();

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

	world->deleteAllShapes();
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	//deux_png.Delete();
	//flag_png.Delete();
	//grass_png.Delete();
	shaderProgram3D->Delete();
	shaderProgram2D->Delete();

	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();

	PlaySound::stopEngine();

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
	
	if (shouldReloadArrays)
	{
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);
	}else
	{
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), &vertices[0]);
	}
	
}

void reloadIndicesInEBO(EBO &EBO1){
	EBO1.Bind();
	if (shouldReloadArrays)
	{
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), &indices[0], GL_DYNAMIC_DRAW);
	}else
	{
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(float), &indices[0]);
	}

}

