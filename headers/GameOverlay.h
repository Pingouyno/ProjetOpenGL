#ifndef GAME_OVERLAY_CLASS_H
#define GAME_OVERLAY_CLASS_H

#include"Overlay.h"
#include"EntityItem.h"

class GameOverlay : public Overlay
{
    private:
        const static int HOTBAR_SIZE;
        vector<Quad2D*> hotBar;
        int activeHotBarSlot;
        Quad2D* slotHighLighter;
        
    
    public:
        GameOverlay(Camera* camera);
        void setupOverlay();
        void updateScoreDisplay(int newScore);

        bool tryStoringItemInHotbar(EntityItem* item);
        int getActiveHotBarSlot();
        void setActiveHotBarSlot(int slot);
        Texture* getTextureFromSlot(int slot);
        Texture* getActiveSlotTexture();
};
#endif