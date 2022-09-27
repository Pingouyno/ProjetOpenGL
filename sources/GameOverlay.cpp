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
	float pngPixelSize = 64.0f; 
	float sizeRatioX = Shape::toXRatio(pngPixelSize);
	float sizeRatioY = Shape::toYRatio(pngPixelSize);
	glm::vec3 pos(-sizeRatioX / 2.0f, -sizeRatioY / 2.0f, 0.0f);
	addStaticShape(new Quad2D(pos, pngPixelSize, pngPixelSize, Texture::get2DImgTexture("crosshair.png")));

	//HOTBAR
	vec2 hotbarPos = vec2(0, -0.8);
	const vec2 offset = vec2(sizeRatioX, 0);
	const vec2 initialPos = hotbarPos - vec2(Shape::toXRatio(pngPixelSize * HOTBAR_SIZE / 2.0f), 0) - offset;

	//pour les comptes de chaque itemstack
	const vec2 countDigitOffset = vec2(sizeRatioX * 0.75f, sizeRatioY * 0.25f);
	const int countDigitFontSize = 13;

	vec2 lastSlotPos = initialPos;
	for (int i = 0 ; i < HOTBAR_SIZE ; i++)
	{
		hotBar.push_back(new Quad2D(lastSlotPos + offset, pngPixelSize, pngPixelSize, Texture::Air->getItemStackTex()));
		
		//counts
		hotBarCountDigits.push_back(new TextEntity2D(vec3(lastSlotPos + offset + countDigitOffset, 0),  countDigitFontSize, ""));
		addTextEntity2D(hotBarCountDigits.back());

		lastSlotPos = hotBar.back()->pos;
	}
	for (Quad2D* ptrShape : hotBar) 
	{
		addStaticShape(ptrShape);
		addStaticShape(new Quad2D(ptrShape->pos, ptrShape->pixWidth, ptrShape->pixHeight, Texture::get2DImgTexture("square.png")));
		this->hotBarItemCounts.push_back(0);
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
		if (hotBar[i]->tex->tex3Did == itemTexture->tex3Did && hotBarItemCounts[i] < MAX_ITEM_STACK_SIZE)
		{
			hotBarItemCounts[i] += 1;
			const int newCountDigit = hotBarItemCounts[i] = stoi(hotBarCountDigits[i]->text) + 1;
			hotBarCountDigits[i]->setText(std::to_string(newCountDigit));
			return true;
		}
	}

	//chercher dans les emplacements vides
	for (int i = 0 ; i < HOTBAR_SIZE ; i++)
	{
		if (hotBar[i]->tex->tex3Did == Texture::TEX3D::AIR)
		{
			hotBar[i]->tex = itemTexture->getItemStackTex();
			hotBarItemCounts[i] += 1;
			//pour éviter stoi("");
			const int newCountDigit = hotBarItemCounts[i] == 1 ? 1 : stoi(hotBarCountDigits[i]->text) + 1;
			hotBarCountDigits[i]->setText(std::to_string(newCountDigit));
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

Texture* GameOverlay::tryGettingCurrentTextureFromHotbar()
{
	if (hotBarItemCounts[activeHotBarSlot] > 0)
	{
		Texture* textureToReturn = getTextureFromSlot(activeHotBarSlot);
		hotBarItemCounts[activeHotBarSlot] -= 1;

		hotBarCountDigits[activeHotBarSlot]->setText(std::to_string(stoi(hotBarCountDigits[activeHotBarSlot]->text) - 1));
		if (hotBarItemCounts[activeHotBarSlot] == 0)
		{
			hotBarCountDigits[activeHotBarSlot]->setText("");
			hotBar[activeHotBarSlot]->tex = Texture::Air->getItemStackTex();
		}
		return textureToReturn;
	}
	else return Texture::Air;
}
