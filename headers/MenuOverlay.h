#ifndef MENU_OVERLAY_CLASS_H
#define MENU_OVERLAY_CLASS_H

#include"Overlay.h"

class MenuOverlay : public Overlay
{
    private:
        //mettre des variables éventuellement

    public:
        TextEntity2D* fpsBar;

        MenuOverlay(Camera* camera);
        void setupOverlay();
};
#endif