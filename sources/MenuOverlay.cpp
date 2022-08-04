#include"../headers/MenuOverlay.h"

//la classe parent contient déjà toutes les instructions nécessaires
MenuOverlay::MenuOverlay(Camera* camera) : Overlay(camera){
	setupOverlay();
}

void MenuOverlay::setupOverlay()
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

	creativeButton->clickLogic = [this, creativeButton, survivalButton](){creativeButton->despawn(); survivalButton->spawn(); gameMode = GameMode::CREATIVE;};
	survivalButton->clickLogic = [this, creativeButton, survivalButton](){survivalButton->despawn(); creativeButton->spawn(); gameMode = GameMode::SURVIVAL; camera->fall();};

	addClickShape(creativeButton);
	addClickShape(survivalButton);
}
