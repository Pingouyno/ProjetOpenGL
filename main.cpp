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

#include"headers/shaderClass.h"
#include"headers/VAO.h" 
#include"headers/VBO.h"
#include"headers/EBO.h"
#include"headers/Texture.h"
#include"headers/Camera.h"
#include"headers/GlobalArrays.h"
#include"headers/Cube.h"

const int width = 1920;
const int heigth = 1080;
const int FRAME_MILLI = 17;

//MÉTHODES UTILISÉES____________________________________________________________

void checkProgramKill(GLFWwindow *window);
void doRenderLogic(GLFWwindow* window, int seed);
void reloadVerticesInVBO(VBO &VBO1);
void reloadIndicesInEBO(EBO &EBO1);

//______________________________________________________________________________

int main()
{	
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
	Shader shaderProgram("resources/shaders/default.vert", "resources/shaders/default.frag"); 

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

	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

	Texture deux_png("resources/textures/deux_icon.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	deux_png.texUnit(shaderProgram, "tex0", 0);

	Texture grass_png("resources/textures/grass.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	grass_png.texUnit(shaderProgram, "tex0", 0);

	Texture flag_png("resources/textures/sky.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	grass_png.texUnit(shaderProgram, "tex0", 0);


	float rotation = 0.0f;
	double prevTime = glfwGetTime();

	glEnable(GL_DEPTH_TEST);

	Camera camera(width, heigth, glm::vec3(0.0f, 0.0f, 0.2f));

	EBO1.Bind();

	auto targetTime = high_resolution_clock::now();
	targetTime += milliseconds(FRAME_MILLI);


	vector<float> cubeColor({1.0f, 0.5f, 0.5f});

	vector<float> pos({2.3f, 2.2f, 2.5f});
	float size = 2.0f;
	Cube c1(pos, size, &grass_png);

	vector<float> pos2({-10.3f, 10.2f, -10.5f});
	float size2 = 1.0f;
	Cube c2(pos2, size2);

	vector<float> pos3({-5.3f, 5.2f, -5.5f});
	float size3 = 4.0f;
	Cube c3(pos3, size3);

	vector<float> pos4({5.0f, -5.0f, 5.0f});
	float size4 = 10.0f;
	Cube c4(pos4, size4, &deux_png);

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
		shaderProgram.Activate();

		camera.Inputs(window);
		camera.Matrix(45.0f, 0.01f, 100.0f, shaderProgram, "camMatrix");

		glUniform1f(uniID, 0.5f);

		// Bind the VAO so OpenGL knows to use it
		VAO1.Bind();
		 
		/*Déplacer la pyramide
		{
			int arr[5] = {0, 8, 16, 24, 32};
			float distX = camera.Position.x - ((vertices[arr[0]] + vertices[arr[2]]) / 2);
			float factX = distX >= 0 ? 1 : -1;
			float distZ = camera.Position.z - ((vertices[arr[0] + 2] + vertices[arr[2] + 2]) / 2);
			float factZ = distZ >= 0 ? 1 : -1;
			for (int i = 0 ; i < 5; i++){
				vertices[arr[i]] += 0.008f * factX;
				vertices[arr[i] + 2] += 0.008f * factZ;
			}
		}
		*/

		reloadVerticesInVBO(VBO1);
		reloadIndicesInEBO(EBO1);
	
		/*dessiner les valeurs hard-codées
		deux_png.Bind();
		glDrawElements(GL_TRIANGLES, 6*3,  GL_UNSIGNED_INT, 0);
		
		deux_png.Unbind();
		flag_png.Bind();
		glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_INT, (void*)(6*3*sizeof(int)));

		flag_png.Unbind();
		grass_png.Bind();
		glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_INT, (void*)(8*3*sizeof(int)));

		
		//grass_png.Unbind();

		c1.moveTo(c1.pos[0] - 0.01f, c1.pos[1], c1.pos[2]);
		c2.setSize(c2.size + 0.001f);
		*/

		grass_png.Bind();

		Shape::renderActiveShapes();

		grass_png.Unbind();

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

	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	deux_png.Delete();
	flag_png.Delete();
	grass_png.Delete();
	
	shaderProgram.Delete();

	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}


void checkProgramKill(GLFWwindow* window){
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS 
			&& glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

void doRenderLogic(GLFWwindow* window, int &seed){

	/*
	const void* falseOffset = (void*)(3*6 * sizeof(int));
	const void* trueOffset = 0;
	
	// Draw the triangle using the GL_TRIANGLES primitive      
									//nombre d'indices à render                    //offset (en octets) de indices[]
	glDrawElements(GL_TRIANGLES, 3*6, GL_UNSIGNED_INT, 0);
						//sizeof(indices)/sizeof(int)     (seed == 0 ? trueOffset : falseOffset)      
    */     
}

void reloadVerticesInVBO(VBO &VBO1){
	//Mettre à jour le tableau et re-charger le VBO. On prend SubData pour optimisation
	VBO1.Bind();
	
	if (Shape::newShapeCreated)
	{
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);
	}else
	{
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), &vertices[0]);
	}
	
}

void reloadIndicesInEBO(EBO &EBO1){
	EBO1.Bind();
	if (Shape::newShapeCreated)
	{
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), &indices[0], GL_DYNAMIC_DRAW);
	}else
	{
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(float), &indices[0]);
	}

}