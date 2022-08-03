#include <iostream>

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

int main()
{
    std::cout << "hey\n\n";

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
	 GLFWwindow* window = glfwCreateWindow(800.0f, 800.0f, "ProjetOpenGL", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
}