//
// Created by romak on 02.02.2025.
//

#ifndef SOUND_H
#define SOUND_H

#include "Setup.h"
struct VolumeAdj {
    int channel;
    int soundchosen;
};

class Sound {
public:
    static std::vector<Mix_Chunk*> SoundVector;  
    static std::vector<VolumeAdj> ChannelPlayingVector;
    static int VolumeAdjustmentChannel[4];
    static void Init();
    static void LoadSound(const char* filename);
    static void VolumeAdjustment();
    static void PlaySound(int soundchoice);
    static void PlaySpecificChannelSound(int soundchoice, int channel);
    static void PlayMusic(const char* filename, int loops);
    static void PlaySoundFade(const char* filename, int channel, int fadeIn, int fadeOut);
    static void PAndUNP_SoundEffects(bool choice);
    static void Cleanup();
};
#endif //SOUND_H
