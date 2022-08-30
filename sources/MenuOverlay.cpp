#include"../headers/MenuOverlay.h"

//la classe parent contient déjà toutes les instructions nécessaires
MenuOverlay::MenuOverlay(Camera* camera) : Overlay(camera){
	setupOverlay();
}

void MenuOverlay::setupOverlay()
{
	Texture* square_png = Texture::get2DImgTexture("square.png");
    Texture* sadge_png = Texture::get2DImgTexture("sadge.png");
	Texture* obama_png = Texture::get2DImgTexture("obama.png");

	//background noir
	addStaticShape(new Quad2D(glm::vec3(-1.0f, -1.0f, 0.0f), screenWidth, screenHeight, square_png));

	//Bouton créatif, on veut que la texture fasse 128 pixels de large peu importe les dimensions de l'écran
	float pixelSize = 128.0f; 
	glm::vec2 pos(-1.0f, 0.5f);
	Quad2D* creativeButton = new Quad2D(pos, pixelSize, pixelSize, sadge_png);
	Quad2D* survivalButton = new Quad2D(pos, pixelSize, pixelSize, obama_png);
	survivalButton->despawn();
	creativeButton->clickLogic = [this, creativeButton, survivalButton](){creativeButton->despawn(); survivalButton->spawn(); gameMode = GameMode::CREATIVE;};
	survivalButton->clickLogic = [this, creativeButton, survivalButton](){survivalButton->despawn(); creativeButton->spawn(); gameMode = GameMode::SURVIVAL; camera->fall();};
	addClickShape(creativeButton);
	addClickShape(survivalButton);

	TextEntity2D* textEntity2D = new TextEntity2D(vec3(0, -0.5, 0), 20, TextEntity2D::DEFAULT_TEXT);
	addTextEntity2D(textEntity2D);

	fpsBar = new TextEntity2D(vec3(-0.6, 0.75, 0), 16, TextEntity2D::DEFAULT_TEXT);
	addTextEntity2D(fpsBar);
}