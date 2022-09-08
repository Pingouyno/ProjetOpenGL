#ifndef MY_GLOBALS_H
#define MY_GLOBALS_H

#include<vector>
using namespace std;

#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"

enum WorldState : int {GAME = 0, MENU = 1};
enum GameMode : int {SURVIVAL = 0, CREATIVE = 1};

// Vertices coordinates
extern std::vector<float> vertices;
extern std::vector<int> indices;
extern const int VIEW_DISTANCE;
extern Shader* shaderProgram2D;
extern Shader* shaderProgram3D;
extern Shader* shaderProgramCube;
extern float screenWidth;
extern float screenHeight;
extern WorldState worldState;
extern GameMode gameMode;
extern void* selectedTextBox;
extern bool shouldCenterCursor;
extern VAO* VAO1;
extern VBO* VBO1;
extern EBO* EBO1;
extern const int CHUNK_SIZE;
extern const int CHUNK_HEIGHT;
extern const int WORLD_CHUNK_COUNT;
extern const int WORLD_SIZE;
extern const int CHUNK_RENDER_DISTANCE;

#endif