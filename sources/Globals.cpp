#include"../headers/Globals.h"

std::vector<float> vertices = {};
std::vector<int> indices = {};

const int VIEW_DISTANCE = 500;

Shader* shaderProgram2D = nullptr;
Shader* shaderProgram3D = nullptr;
Shader* shaderProgramCube = nullptr;

float screenWidth = 1420.0f;
float screenHeight = 900.0f;

WorldState worldState = GAME;
GameMode gameMode = SURVIVAL;

//TextEntity2D*
void* selectedTextBox = nullptr;

bool shouldCenterCursor = true;

VAO* VAO1 = nullptr;
VBO* VBO1 = nullptr;
EBO* EBO1 = nullptr;

const int CHUNK_SIZE = 16;
const int CHUNK_HEIGHT = 64;
const int WORLD_CHUNK_COUNT = 200;
const int WORLD_SIZE = WORLD_CHUNK_COUNT * CHUNK_SIZE; 
const int CHUNK_RENDER_DISTANCE = 8;
