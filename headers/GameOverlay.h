#ifndef GAME_OVERLAY_CLASS_H
#define GAME_OVERLAY_CLASS_H

#include"Overlay.h"

class GameOverlay : public Overlay
{
    private:
        //score va de gauche à droite (0 à n)
        vector<Quad2D*> scoreQuads;
    
    public:
        GameOverlay(Camera* camera);
        void setupOverlay();
        void updateScoreDisplay(int newScore);
};
#endif