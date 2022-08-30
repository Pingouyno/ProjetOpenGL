#ifndef GL_BUFFER_MANAGER_CLASS_H
#define GL_BUFFER_MANAGER_CLASS_H

#include"../libraries/include/GLAD/glad.h"
#include"Globals.h"
#include"Block.h"

//classe qui manage les index de buffer pour les Blocks
class GLBufferManager
{
    private:
        static vector<int> availableIndiceSlots;
        static vector<int> availableVerticeSlots;
        static void popLastAvailableSlot();

    public:
        static int getFirstAvailableSlotInVertices();
        static int getFirstAvailableSlotInIndices();
        static void mallocBlockIntoGLBuffer(Block* block);
        static void freeBlockFromGLBuffer(Block* block);
};

#endif