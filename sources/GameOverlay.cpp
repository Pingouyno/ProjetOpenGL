#include"../headers/GameOverlay.h"

//la classe parent contient déjà toutes les instructions nécessaires
GameOverlay::GameOverlay(Camera* camera) : Overlay(camera){
    setupOverlay();
}

void GameOverlay::setupOverlay()
{
	Texture* crosshair_png = new Texture("resources/textures/crosshair.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	(*crosshair_png).texUnit(*shaderProgram2D, "tex0", 0);

	//crosshair
	float pixelSize = 64.0f; 
	float sizeRatioX = Shape::toXRatio(pixelSize);
	float sizeRatioY = Shape::toYRatio(pixelSize);
	glm::vec3 pos(-sizeRatioX / 2.0f, -sizeRatioY / 2.0f, 0.0f);
	addStaticShape(new Quad2D(pos, sizeRatioX, sizeRatioY, crosshair_png, [](){}));

	//nombres
	scoreQuads = 
	{
		new Quad2D(glm::vec3(-1.0f, -1.0f, 0.0f), 65, 77, TextManager::getNumberTexture(0)),
		new Quad2D(glm::vec3(-0.8f, -1.0f, 0.0f), 65, 77, TextManager::getNumberTexture(0)),
		new Quad2D(glm::vec3(-0.6f, -1.0f, 0.0f), 65, 77, TextManager::getNumberTexture(0)),
		new Quad2D(glm::vec3(-0.4f, -1.0f, 0.0f), 65, 77, TextManager::getNumberTexture(0))
	};
	for (Quad2D* ptrShape : scoreQuads) addStaticShape(ptrShape);
	
}

void GameOverlay::updateScoreDisplay(int newScore)
{
	//chaque case prend un chiffre (unité, dizaine, etc...)
	scoreQuads[3]->tex = TextManager::getNumberTexture(newScore % 10);
	newScore = newScore / 10;
    scoreQuads[2]->tex = TextManager::getNumberTexture(newScore % 10);
	newScore = newScore / 10;
	scoreQuads[1]->tex = TextManager::getNumberTexture(newScore % 10);
	newScore = newScore / 10;
	scoreQuads[0]->tex = TextManager::getNumberTexture(newScore % 10);
}

