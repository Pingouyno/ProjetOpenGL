#include"../headers/GLBufferManager.h"

vector<int> GLBufferManager::availableIndiceSlots = {};
vector<int> GLBufferManager::availableVerticeSlots = {};

int GLBufferManager::getFirstAvailableSlotInVertices()
{
    if (availableVerticeSlots.size() == 0) return vertices.size();
    else return availableVerticeSlots.back();
}

int GLBufferManager::getFirstAvailableSlotInIndices()
{
    if (availableIndiceSlots.size() == 0) return indices.size();
    else return availableIndiceSlots.back();
}

void GLBufferManager::popLastAvailableSlot()
{
    if (availableVerticeSlots.size() > 0) availableVerticeSlots.pop_back();
    if (availableIndiceSlots.size() > 0) availableIndiceSlots.pop_back();
}

//modifier indexInVertices et indexInIndices
void GLBufferManager::mallocBlockIntoGLBuffer(Block* block)
{
     //i = index du tableau, en numérotation de chaque vertex dans vertices[]
    block -> indexInVertices = getFirstAvailableSlotInVertices();  //chaque vertex contient (x, y, z) + (r, g, b) + (mapX, mapY) = 8
    block -> indexInIndices = getFirstAvailableSlotInIndices();
    popLastAvailableSlot();
}

void GLBufferManager::freeBlockFromGLBuffer(Block* block)
{
    availableVerticeSlots.push_back(block->indexInVertices);
    availableIndiceSlots.push_back(block->indexInIndices);
    block->indexInVertices = -1;
    block->indexInIndices = -1;
}