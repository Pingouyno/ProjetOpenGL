#ifndef GAME_OVERLAY_CLASS_H
#define GAME_OVERLAY_CLASS_H

#include"Overlay.h"
#include"EntityItem.h"
#include"TextEntity2D.h"

class GameOverlay : public Overlay
{
    private:
        const static int MAX_ITEM_STACK_SIZE = 64;
        const static int HOTBAR_SIZE;
        vector<Quad2D*> hotBar;
        vector<TextEntity2D*> hotBarCountDigits;
        vector<int> hotBarItemCounts;
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
        Texture* tryGettingCurrentTextureFromHotbar();
};
#endif