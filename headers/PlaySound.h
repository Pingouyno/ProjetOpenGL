#ifndef SOUND_CLASS_H
#define SOUND_CLASS_H

#include<string>
using namespace std;
#include"../libraries/include/IrrKlang/include/irrKlang.h"
using namespace irrklang;
#include"Globals.h"

class PlaySound {

    public:
        const static string SOUND_PATH;
        static ISoundEngine* soundEngine;

        static void startEngine();
        static void stopEngine();

        //jouer des sons
        static void playLandSound();
        static void playSnowManHitSound(glm::vec3 &snowmanPos);
        static void playPlayerHitSound();
        static void playBlockBreakSound();
};
#endif