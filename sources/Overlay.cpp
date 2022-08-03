#include"../headers/Overlay.h"

vector<Shape*> staticShapes({});
vector<Shape*> clickableShapes({});

Overlay::Overlay(){}

Overlay::Overlay(Camera *camera, int worldStateType)
{
    this->camera = camera;

    switch(worldStateType){
        case 0:
            setupGameOverlay();
            break;

        case 1:
            setupMenuOverlay();
            break;
    }
}

void Overlay::render()
{
    for (Quad2D* ptrShape : staticShapes)
    {
        if (ptrShape->active)
        {
            ptrShape->render();
        }
    }

    for (Quad2D* ptrShape : clickableShapes)
    {
        if (ptrShape->active)
        {
            ptrShape->render();
        }
    }
}

void Overlay::checkCollisions(glm::vec3 &mousePos)
{
    for (Quad2D* ptrShape : Quad2D::shapesHUDCollidable)
    {
        if (ptrShape->active && ptrShape->isColliding(mousePos))
        {
            ptrShape->doClickLogic();

            /*on échappe car on ne cliquer qu'un seul icône à la fois.
                de plus, si un des boutons réactive le suivant dans la liste, 
                on peut avoir un comportement non défini (ré-activation des boutons perpétuelle)*/
            break;
        }
    }
}

void Overlay::addStaticShape(Quad2D* shape)
{
    staticShapes.push_back(shape);
}

void Overlay::addClickShape(Quad2D* shape)
{
    clickableShapes.push_back(shape);
}

const void Overlay::setupGameOverlay()
{
	Texture* crosshair_png = new Texture("resources/textures/crosshair.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	(*crosshair_png).texUnit(*shaderProgram2D, "tex0", 0);

	//pour que la texture fasse 64 pixels de large peu importe les dimensions de l'écran
	float pixelSize = 64.0f; 
	float sizeRatioX = Shape::toXRatio(pixelSize);
	float sizeRatioY = Shape::toYRatio(pixelSize);
	glm::vec3 pos(-sizeRatioX / 2.0f, -sizeRatioY / 2.0f, 0.0f);

	addStaticShape(new Quad2D(pos, sizeRatioX, sizeRatioY, crosshair_png, [](){}));

    //chiffres
	addStaticShape(new Quad2D(glm::vec3(-1.0f, -1.0f, 0.0f), 65, 77, TextManager::getNumberTexture('0')));
	addStaticShape(new Quad2D(glm::vec3(-0.8f, -1.0f, 0.0f), 65, 77, TextManager::getNumberTexture('1')));
}


const void Overlay::setupMenuOverlay()
{
	Texture* square_png = new Texture("resources/textures/square.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	square_png->texUnit(*shaderProgram2D, "tex0", 0);

    Texture* sadge_png = new Texture("resources/textures/sadge.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	sadge_png->texUnit(*shaderProgram2D, "tex0", 0);

	Texture* obama_png = new Texture("resources/textures/obama.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	obama_png->texUnit(*shaderProgram2D, "tex0", 0);

	addStaticShape(new Quad2D(glm::vec3(-1.0f, -1.0f, 0.0f), screenWidth, screenHeight, square_png));

	//pour que la texture fasse 128 pixels de large peu importe les dimensions de l'écran
	float pixelSize = 128.0f; 
	float sizeRatioX = Shape::toXRatio(pixelSize);
	float sizeRatioY = Shape::toYRatio(pixelSize);
	glm::vec2 pos(-1.0f, 0.5f);

	Quad2D* creativeButton = new Quad2D(pos, pixelSize, pixelSize, sadge_png, [](){});
	Quad2D* survivalButton = new Quad2D(pos, pixelSize, pixelSize, obama_png, [](){});

	survivalButton->despawn();

	creativeButton->clickLogic = [this, creativeButton, survivalButton](){creativeButton->despawn(); survivalButton->spawn(); camera->isInCreative = true;};
	survivalButton->clickLogic = [this, creativeButton, survivalButton](){survivalButton->despawn(); creativeButton->spawn(); camera->isInCreative = false; camera->fall();};

	addClickShape(creativeButton);
	addClickShape(survivalButton);
}