#ifndef MY_GLOBALS_H
#define MY_GLOBALS_H

#include<vector>
#include"shaderClass.h"
using namespace std;

// Vertices coordinates
extern std::vector<float> vertices;
extern std::vector<int> indices;

extern const int LAB_SIZE;
extern vector<int> labyrinth;
extern bool shouldReloadArrays;
extern Shader* shaderProgram2D;
extern Shader* shaderProgram3D;
extern float screenWidth;
extern float screenHeight;

#endif