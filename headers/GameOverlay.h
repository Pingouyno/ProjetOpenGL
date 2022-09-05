#ifndef GAME_OVERLAY_CLASS_H
#define GAME_OVERLAY_CLASS_H

#include"Overlay.h"

class GameOverlay : public Overlay
{
    private:
        vector<Quad2D*> hotBar;
        Quad2D* slotHighLighter;
        
    
    public:
        GameOverlay(Camera* camera);
        void setupOverlay();
        void updateScoreDisplay(int newScore);

        void setActiveHotBarSlot(int slot);
        Texture* getTextureFromSlot(int slot);
};
#endif