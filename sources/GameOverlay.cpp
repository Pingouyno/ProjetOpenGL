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

	//HOTBAR
	int hotbarSize = 2;
	vec2 hotbarPos = vec2(0, -0.8);

	Quad2D* slot0 = new Quad2D(hotbarPos - vec2(Shape::toXRatio(pixelSize * hotbarSize / 2), 0), pixelSize, pixelSize, Texture::get2DImgTexture("dirt.png"));
	Quad2D* slot1 = new Quad2D(vec2(slot0->pos) + vec2(Shape::toXRatio(pixelSize), 0), pixelSize, pixelSize, Texture::get2DImgTexture("stone.png"));
	hotBar = 
	{
		slot0,
		slot1
	};
	for (Quad2D* ptrShape : hotBar) 
	{
		addStaticShape(ptrShape);
		addStaticShape(new Quad2D(ptrShape->pos, ptrShape->pixWidth, ptrShape->pixHeight, Texture::get2DImgTexture("square.png")));
	}

	slotHighLighter = new Quad2D(hotBar[0]->pos, hotBar[0]->pixWidth, hotBar[0]->pixHeight, Texture::get2DImgTexture("yellow.png"));
	addStaticShape(slotHighLighter);
}

void GameOverlay::setActiveHotBarSlot(int slot)
{
	slotHighLighter->moveTo(hotBar[slot]->pos);
}

Texture* GameOverlay::getTextureFromSlot(int slot)
{
	switch (slot)
	{
		case 0:
			return Texture::get3DImgTexture(Texture::DIRT);
			break;
		
		case 1:
			return Texture::get3DImgTexture(Texture::STONE);;
			break;
	}
	cout << "\n\n**ERREUR** : slot out of bounds!\n\n";
	throw 1;
	return nullptr;
}
