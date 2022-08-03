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
		printf("Engin audio impossible à démarrer\n");
		throw 0;
	}
}

//arrêter l'engin irrKlang pour le son
void PlaySound::stopEngine()
{
	//arrêter l'engin de son
	soundEngine->drop();
}

void PlaySound::playJumpSound()
{
    string fileName = "jump_01.wav";
    soundEngine->play2D((SOUND_PATH + fileName).c_str());
}