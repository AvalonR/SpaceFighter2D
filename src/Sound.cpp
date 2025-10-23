#include "Sound.h"
#include "Enemy.h"
#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_properties.h"
#include <SDL3_mixer/SDL_mixer.h>
#include <algorithm>
#include <cmath>
#include <utility>

MIX_Mixer *Sound::mixer = nullptr;
std::vector<MIX_Audio *> Sound::SoundVector;
std::vector<MIX_Track *> Sound::TrackVector;
float Sound::VolumeAdjustmentChannel[4];

void Sound::Init() {
  if (!MIX_Init()) {
    std::cout << "Error initializing SDL_mixer: " << SDL_GetError()
              << std::endl;
    return;
  }

  SDL_AudioSpec spec;
  spec.freq = 48000;
  spec.channels = 2;
  spec.format = SDL_AUDIO_S32BE;

  mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
  if (!mixer) {
    std::cout << "Error opening audio device: " << SDL_GetError() << std::endl;
    return;
  }

  TrackVector.reserve(96);
  for (int i = 0; i < 96; i++) {
    MIX_Track *track = MIX_CreateTrack(mixer);
    if (track) {
      TrackVector.push_back(track);

      if (i == 0) {
        MIX_TagTrack(track, TAG_MUSIC);
      }
    }
  }

  LoadSound("playerlaserFO.wav");
  LoadSound("enemylaserFO.wav");
  LoadSound("shipexplosionFO.wav");
  LoadSound("hitFO.wav");
  LoadSound("buttonclickFO.wav");
  LoadSound("coin_drop2FO.wav");
  LoadSound("coin_recieved2FO.wav");
  LoadSound("lasercharge_denoised.mp3");
  LoadSound("laserBuzzFPFO.wav");
  LoadSound("earnxp_normalizedFO.wav");
  LoadSound("levelup_normalizedFO.wav");
  LoadSound("message_normalizedFO.wav");
  LoadSound("sparkFFO.wav");
  LoadSound("sparkSFO.wav");
  LoadSound("reloadFO.wav");
  LoadSound("reloadLowP.wav");

  VolumeAdjustmentChannel[0] = 0.6f; // Master
  VolumeAdjustmentChannel[1] = 0.3f; // Music
  VolumeAdjustmentChannel[2] = 0.4f; // UI
  VolumeAdjustmentChannel[3] = 0.3f; // Player/Enemy

  // Apply initial volumes using tags
  MIX_SetMasterGain(mixer, VolumeAdjustmentChannel[0]);
  MIX_SetTagGain(mixer, TAG_MUSIC, VolumeAdjustmentChannel[1]);
  MIX_SetTagGain(mixer, TAG_UI, VolumeAdjustmentChannel[2]);
  MIX_SetTagGain(mixer, TAG_GAME, VolumeAdjustmentChannel[3]);
}

void Sound::LoadSound(const char *filename) {
  std::string filenamestr(filename);
  std::string assetspath = Setup::basePath + "assets/" + filenamestr;

  MIX_Audio *soundEffect = MIX_LoadAudio(mixer, assetspath.c_str(), false);
  if (!soundEffect) {
    std::cout << "Error loading sound effect: " << SDL_GetError() << std::endl
              << assetspath << std::endl;
    return;
  } else {
    std::cout << "Sound Loaded -> " << filename << std::endl;
    SoundVector.push_back(soundEffect);
  }
}

void Sound::VolumeAdjustment() {
  MIX_SetTagGain(mixer, TAG_MUSIC, VolumeAdjustmentChannel[1]);
  MIX_SetTagGain(mixer, TAG_UI, VolumeAdjustmentChannel[2]);
  MIX_SetTagGain(mixer, TAG_GAME, VolumeAdjustmentChannel[3]);
}
void Sound::PlaySound(int soundchoice) {
  if (soundchoice >= SoundVector.size()) {
    return;
  }

  int channel = -1;
  for (int i = 1; i < TrackVector.size(); i++) {
    if (!MIX_TrackPlaying(TrackVector[i])) {
      channel = i;
      break;
    }
  }

  if (channel == -1) {
    return;
  }

  const char *tag = nullptr;
  if (soundchoice == 4 || soundchoice == 11 || soundchoice == 10 ||
      soundchoice == 9) {
    tag = TAG_UI;
  } else if (soundchoice == 0 || soundchoice == 5 || soundchoice == 6 ||
             soundchoice == 14 || soundchoice == 1 || soundchoice == 2 ||
             soundchoice == 3 || soundchoice == 7 || soundchoice == 8 ||
             soundchoice == 12 || soundchoice == 13 || soundchoice == 15) {
    tag = TAG_GAME;
  }

  if (tag) {
    MIX_UntagTrack(TrackVector[channel], TAG_UI);
    MIX_UntagTrack(TrackVector[channel], TAG_GAME);
    MIX_TagTrack(TrackVector[channel], tag);
  }

  MIX_SetTrackAudio(TrackVector[channel], SoundVector[soundchoice]);
  if (!MIX_PlayTrack(TrackVector[channel], 0)) {
    std::cout << "Error playing sound effect: " << SDL_GetError() << std::endl;
  }
}

void Sound::PlaySpecificChannelSound(int soundchoice, int channel) {
  if (soundchoice >= SoundVector.size() || channel >= TrackVector.size()) {
    std::cout << "Invalid sound or channel index" << std::endl;
    return;
  }

  const char *tag = nullptr;
  if (soundchoice == 4 || soundchoice == 11 || soundchoice == 10 ||
      soundchoice == 9) {
    tag = TAG_UI;
  } else if (soundchoice == 0 || soundchoice == 5 || soundchoice == 6 ||
             soundchoice == 14 || soundchoice == 1 || soundchoice == 2 ||
             soundchoice == 3 || soundchoice == 7 || soundchoice == 8 ||
             soundchoice == 12 || soundchoice == 13 || soundchoice == 15) {
    tag = TAG_GAME;
  }

  if (tag) {
    MIX_UntagTrack(TrackVector[channel], TAG_UI);
    MIX_UntagTrack(TrackVector[channel], TAG_GAME);
    MIX_TagTrack(TrackVector[channel], tag);
  }

  MIX_SetTrackAudio(TrackVector[channel], SoundVector[soundchoice]);
  if (!MIX_PlayTrack(TrackVector[channel], 0)) {
    std::cout << "Error playing sound effect: " << SDL_GetError() << std::endl;
  }
}

void Sound::PlayMusic(const char *filename, int loops) {
  std::string filepath = Setup::basePath + "assets/" + filename;

  if (MIX_TrackPlaying(TrackVector[0])) {
    MIX_StopTrack(TrackVector[0], 0);
  }

  MIX_Audio *music = MIX_LoadAudio(mixer, filepath.c_str(), false);
  if (!music) {
    std::cout << "Error loading music: " << SDL_GetError() << filepath
              << std::endl;
    return;
  }

  if (!MIX_SetTrackAudio(TrackVector[0], music)) {
    std::cout << "Error setting track audio: " << SDL_GetError() << std::endl;
  }

  SDL_PropertiesID props = SDL_CreateProperties();
  SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loops);

  if (!MIX_PlayTrack(TrackVector[0], props)) {
    std::cout << "Error playing music: " << SDL_GetError() << std::endl;
  }

  SDL_DestroyProperties(props);
}

void Sound::RandomizeMusic() {
  int musicChoice = Enemy::Randomizer(1, 4);
  if (musicChoice == 1) {
    Sound::PlayMusic("space-158081.mp3", -1);
  }
  if (musicChoice == 2) {
    Sound::PlayMusic("Aylex-AI.mp3", -1);
  }
  if (musicChoice == 3) {
    Sound::PlayMusic("Aylex-FF.mp3", -1);
  }
  if (musicChoice == 4) {
    Sound::PlayMusic("ocean-wave-ambient.mp3", -1);
  }
}

void Sound::PAndUNP_SoundEffects(bool choice) {
  if (choice) {
    MIX_PauseTag(mixer, TAG_GAME);
  } else {
    MIX_ResumeTag(mixer, TAG_GAME);
  }
}
void Sound::StopGameSound() { MIX_StopTag(mixer, TAG_GAME, 0); }

void Sound::Cleanup() {
  for (auto &soundEffect : SoundVector) {
    MIX_DestroyAudio(soundEffect);
  }
  SoundVector.clear();
  TrackVector.clear();

  if (mixer) {
    MIX_DestroyMixer(mixer);
    mixer = nullptr;
  }

  MIX_Quit();
}
