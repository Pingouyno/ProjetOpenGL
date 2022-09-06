#include"../headers/GameOverlay.h"

const int GameOverlay::HOTBAR_SIZE = 9;

//la classe parent contient déjà toutes les instructions nécessaires
GameOverlay::GameOverlay(Camera* camera) : Overlay(camera){
    setupOverlay();
	this->activeHotBarSlot = 0;
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
	vec2 hotbarPos = vec2(0, -0.8);
	const vec2 offset = vec2(Shape::toXRatio(pixelSize), 0);
	const vec2 initialPos = hotbarPos - vec2(Shape::toXRatio(pixelSize * HOTBAR_SIZE / 2.0f), 0) - offset;

	vec2 lastSlotPos = initialPos;
	for (int i = 0 ; i < HOTBAR_SIZE ; i++)
	{
		hotBar.push_back(new Quad2D(lastSlotPos + offset, pixelSize, pixelSize, Texture::Air->getItemStackTex()));
		lastSlotPos = hotBar.back()->pos;
	}
	for (Quad2D* ptrShape : hotBar) 
	{
		addStaticShape(ptrShape);
		addStaticShape(new Quad2D(ptrShape->pos, ptrShape->pixWidth, ptrShape->pixHeight, Texture::get2DImgTexture("square.png")));
	}

	slotHighLighter = new Quad2D(hotBar[0]->pos, hotBar[0]->pixWidth, hotBar[0]->pixHeight, Texture::get2DImgTexture("yellow.png"));
	addStaticShape(slotHighLighter);
}

//retourne true si on a trouvé une place dans l'inventaire. cherche d'abord le même type de texture, PUIS les places vides
bool GameOverlay::tryStoringItemInHotbar(EntityItem* item)
{
	Texture* itemTexture = item->itemCube->tex;

	//chercher à regrouper les textures
	for (int i = 0 ; i < HOTBAR_SIZE ; i++)
	{
		if (hotBar[i]->tex->tex3Did == itemTexture->tex3Did)
		{
			return true;
		}
	}

	//chercher dans les emplacements vides
	for (int i = 0 ; i < HOTBAR_SIZE ; i++)
	{
		if (hotBar[i]->tex->tex3Did == Texture::TEX3D::AIR)
		{
			hotBar[i]->tex = itemTexture->getItemStackTex();
			return true;
		}
	}
	return false;
}

int GameOverlay::getActiveHotBarSlot()
{
	return activeHotBarSlot;
}

void GameOverlay::setActiveHotBarSlot(int slot)
{
	this->activeHotBarSlot = slot;
	slotHighLighter->moveTo(hotBar[slot]->pos);
}

Texture* GameOverlay::getTextureFromSlot(int slot)
{
	return Texture::get3DImgTexture(hotBar[slot]->tex->tex3Did);
}

Texture* GameOverlay::getActiveSlotTexture()
{
	return getTextureFromSlot(activeHotBarSlot);
}
