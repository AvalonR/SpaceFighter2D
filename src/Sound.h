#ifndef SOUND_H
#define SOUND_H

#include <SDL3_mixer/SDL_mixer.h>
#include <string>
#include <vector>

class Sound {
public:
  static void Init();
  static void LoadSound(const char *filename);
  static void VolumeAdjustment();
  static void PlaySound(int soundchoice);
  static void PlaySpecificChannelSound(int soundchoice, int channel);
  static void PlayMusic(const char *filename, int loops);
  static void RandomizeMusic();
  static void PAndUNP_SoundEffects(bool choice);
  static void StopGameSound();
  static void Cleanup();

  static constexpr const char *TAG_MUSIC = "music";
  static constexpr const char *TAG_UI = "ui";
  static constexpr const char *TAG_GAME = "game";

  static std::vector<MIX_Audio *> SoundVector;
  static std::vector<MIX_Track *> TrackVector;
  static float
      VolumeAdjustmentChannel[4]; // [0]=Master, [1]=Music, [2]=UI, [3]=Game
  static MIX_Mixer *mixer;
};

#endif
