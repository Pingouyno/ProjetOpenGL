#include"../headers/PlaySound.h"

const string PlaySound::SOUND_PATH = "resources/sounds/";
ISoundEngine* PlaySound::soundEngine = nullptr;

//démarrer l'engin irrKlang pour le son
void PlaySound::startEngine()
{
	soundEngine = createIrrKlangDevice();

    //vérifier que l'engin a pu démarrer, donc ptr != nullptr
	if (soundEngine == nullptr)
	{
		printf("\n\nEngin audio impossible à démarrer\n\n");
		throw 0;
	}
}

//arrêter l'engin irrKlang pour le son
void PlaySound::stopEngine()
{
	//arrêter l'engin de son
	soundEngine->drop();
}

void PlaySound::playLandSound()
{
	string fileName = "mob_horse_land.ogg";
    soundEngine->play2D((SOUND_PATH + fileName).c_str());
}

void PlaySound::playSnowManHitSound(glm::vec3 &snowmanPos)
{
	string fileName = "mob_villager_hit1.ogg";

	if (glm::distance(snowmanPos, *externPlayerPos) <= HEARING_RANGE)
	{
    	soundEngine->play2D((SOUND_PATH + fileName).c_str());
	}
}

void PlaySound::playPlayerHitSound()
{
	string fileName = "damage_hit1.ogg";
    soundEngine->play2D((SOUND_PATH + fileName).c_str());
}

void PlaySound::playBlockBreakSound()
{
	string fileName = "dig_stone4.ogg";
    soundEngine->play2D((SOUND_PATH + fileName).c_str());
}