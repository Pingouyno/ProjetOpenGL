#ifndef MY_GLOBALS_H
#define MY_GLOBALS_H

#include<vector>
using namespace std;

#include"shaderClass.h"
#include"VAO.h"

enum WorldState : int {GAME = 0, MENU = 1};
enum GameMode : int {SURVIVAL = 0, CREATIVE = 1};

// Vertices coordinates
extern std::vector<float> vertices;
extern std::vector<int> indices;

extern const int VIEW_DISTANCE;
extern const int LAB_SIZE;
extern vector<int> labyrinth;
extern bool shouldReloadArrays;
extern Shader* shaderProgram2D;
extern Shader* shaderProgram3D;
extern Shader* shaderProgramCube;
extern float screenWidth;
extern float screenHeight;
extern WorldState worldState;
extern GameMode gameMode;
extern void* selectedTextBox;

#endif