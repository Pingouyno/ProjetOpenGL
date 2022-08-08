#include"../headers/GameOverlay.h"


//la classe parent contient déjà toutes les instructions nécessaires
GameOverlay::GameOverlay(Camera* camera) : Overlay(camera){
    setupOverlay();
}

void GameOverlay::setupOverlay()
{
	//crosshair
	float pixelSize = 64.0f; 
	float sizeRatioX = Shape::toXRatio(pixelSize);
	float sizeRatioY = Shape::toYRatio(pixelSize);
	glm::vec3 pos(-sizeRatioX / 2.0f, -sizeRatioY / 2.0f, 0.0f);
	addStaticShape(new Quad2D(pos, pixelSize, pixelSize, Texture::get2DImgTexture("crosshair.png")));

	//nombres
	Quad2D* score = new Quad2D(glm::vec2(-0.196f, 0.82f), TextManager::getTextTexture("score"));
	Quad2D* numThous = new Quad2D(glm::vec2(score->pos[0] + score->width, score->pos[1]), TextManager::getNumberTexture(0));
	Quad2D* numHundr = new Quad2D(glm::vec2(numThous->pos[0] + numThous->width, score->pos[1]), TextManager::getNumberTexture(0));
	Quad2D* numTenth = new Quad2D(glm::vec2(numHundr->pos[0] + numHundr->width, score->pos[1]), TextManager::getNumberTexture(0));
	Quad2D* numUnity = new Quad2D(glm::vec2(numTenth->pos[0] + numTenth->width, score->pos[1]), TextManager::getNumberTexture(0));
	scoreQuads = 
	{	
		score,
		numThous,
		numHundr,
		numTenth,
		numUnity
	};
	for (Quad2D* ptrShape : scoreQuads) addStaticShape(ptrShape);
}

void GameOverlay::updateScoreDisplay(int newScore)
{
	//chaque case prend un chiffre (unité, dizaine, etc...)
	scoreQuads[4]->tex = TextManager::getNumberTexture(newScore % 10);
	newScore = newScore / 10;
    scoreQuads[3]->tex = TextManager::getNumberTexture(newScore % 10);
	newScore = newScore / 10;
	scoreQuads[2]->tex = TextManager::getNumberTexture(newScore % 10);
	newScore = newScore / 10;
	scoreQuads[1]->tex = TextManager::getNumberTexture(newScore % 10);
}

