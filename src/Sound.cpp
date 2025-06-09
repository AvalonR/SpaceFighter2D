//
// Created by romak on 02.02.2025.
//

#include "Sound.h"
#include "TextureManager.h"
#include <utility>
Mix_Music* music = nullptr;
std::vector<Mix_Chunk*> Sound::SoundVector;
std::vector<VolumeAdj> Sound::ChannelPlayingVector;
int Sound::VolumeAdjustmentChannel[4];

void Sound::Init()
{
    SDL_AudioSpec spec;
    spec.freq = 48000;
    spec.channels = 2;
    spec.format = SDL_AUDIO_S16BE;
    if (!Mix_OpenAudio(0, &spec))
    {
        std::cout << "Error opening audio device channel" << SDL_GetError() << std::endl;
        return;
    }
    Mix_AllocateChannels(64);
    Mix_Volume(-1, 40);
    Mix_VolumeMusic(20);
    LoadSound("playerlaser.wav");
    LoadSound("enemylaser.wav");
    LoadSound("shipexplosion.wav");
    LoadSound("hit.wav");
    LoadSound("buttonclick.wav");
    LoadSound("coin_drop2.wav");
    LoadSound("coin_recieved2.wav");
    LoadSound("lasercharge_normalized.wav");
    LoadSound("laserbeam_normalized.wav");
    LoadSound("earnxp_normalized.wav");
    LoadSound("levelup_normalized.wav");
    LoadSound("message_normalized.wav");
    Mix_ReserveChannels(1);
    VolumeAdjustmentChannel[0] = 104;
    VolumeAdjustmentChannel[1] = 34;
    VolumeAdjustmentChannel[2] = 34;
    VolumeAdjustmentChannel[3] = 34;
}

void Sound::LoadSound(const char* filename) {
    std::string filenamestr(filename);
    std::string assetspath = Setup::basePath + "assets/" + filenamestr;
    Mix_Chunk* soundEffect = Mix_LoadWAV(assetspath.c_str());
    if (!soundEffect)
    {
        std::cout << "Error loading sound effect" << std::endl << assetspath << std::endl;
        return;
    }
    else {
        std::cout << "Sound Loaded -> " << filename << std::endl;
        SoundVector.push_back(soundEffect);
    }
}
void Sound::VolumeAdjustment() {

    std::unordered_map<int, int> soundInstanceCount;
    // First pass: count how many of each sound ID are playing
    for (const auto& v : ChannelPlayingVector) {
        soundInstanceCount[v.soundchosen]++;
    }
    // Second pass: set volume per sound instance, scaled to avoid stacking volume
    for (auto& v : ChannelPlayingVector) {
        int count = soundInstanceCount[v.soundchosen];
        int baseVolume = 0;

        // Pick the base volume level based on category
        if (v.soundchosen == 4 || v.soundchosen == 11 || v.soundchosen == 10 || v.soundchosen == 9) {
            baseVolume = VolumeAdjustmentChannel[1];
        } else if (v.soundchosen == 0 || v.soundchosen == 5 || v.soundchosen == 6) {
            baseVolume = VolumeAdjustmentChannel[2];
        } else if (v.soundchosen == 1 || v.soundchosen == 2 || v.soundchosen == 3 || v.soundchosen == 7 || v.soundchosen == 8 ) {
            baseVolume = VolumeAdjustmentChannel[3];
        }

        // Scale the volume down based on number of concurrent plays
        int scaledVolume = baseVolume;
        if (count > 1) {
            scaledVolume = baseVolume / count;
        }

        Mix_Volume(v.channel, scaledVolume);
    }
    ChannelPlayingVector.erase(
    std::remove_if(ChannelPlayingVector.begin(), ChannelPlayingVector.end(),
        [](const VolumeAdj& v) {
            if (Mix_Playing(v.channel) != 0) {
                return true;
            }
            return false;
        }),
    ChannelPlayingVector.end()
);
}

void Sound::PlaySound(int soundchoice)
{
    int channel = Mix_PlayChannel(-1, SoundVector[soundchoice], 0);
    if (channel == -1)
    {
        std::cout << "Error playing sound effect" << std::endl;
    }
    else {
        ChannelPlayingVector.emplace_back(channel, soundchoice);
    }
}
void Sound::PlaySpecificChannelSound(int soundchoice, int channel)
{
    if (Mix_PlayChannel(channel, SoundVector[soundchoice], 0) == -1)
    {
        std::cout << "Error playing sound effect" << std::endl;
    }
}
void Sound::PlayMusic(const char* filename, int loops)
{
    Mix_VolumeMusic(64);
    std::string DirDest = getcwd(nullptr, 0);
    auto first = DirDest.end() - 17;
    auto last = DirDest.end();
    DirDest.erase(first, last);
    std::replace(DirDest.begin(), DirDest.end(), '\\', '/');
    std::string assetspath = "A:/C++/Game2D/assets/";
    std::string filepath = assetspath + filename;
    // std::cout << filepath << std::endl;
    music = Mix_LoadMUS(filepath.c_str());
    if (!music)
    {
        std::cout << "Error loading sound effect" << std::endl;
        return;
    }
    Mix_PlayMusic(music, loops);
}
void Sound::PlaySoundFade(const char* filename, int channel, int fadeIn, int fadeOut)
{
    std::string DirDest = getcwd(nullptr, 0);
    auto first = DirDest.end() - 17;
    auto last = DirDest.end();
    DirDest.erase(first, last);
    std::replace(DirDest.begin(), DirDest.end(), '\\', '/');
    std::string assetspath = "A:/C++/Game2D/assets/";
    std::string filepath = assetspath + filename;
    // std::cout << filepath << std::endl;
    Mix_Chunk* soundEffect = Mix_LoadWAV(filepath.c_str());
    if (!soundEffect)
    {
        std::cout << "Error loading sound effect" << std::endl;
        return;
    }
    if (Mix_FadeInChannel(channel,soundEffect, 0,fadeIn) == -1)
    {
        std::cout << "Error playing sound effect" << std::endl;
    }
}
void Sound::PAndUNP_SoundEffects(bool choice)
{
    for (int i = 1; i < 64; i++)
    {
        if (choice)
        {
            Mix_Pause(i);
        }
        else
        {
            Mix_Resume(i);
        }
    }
}
void Sound::Cleanup()
{
    for (auto &soundEffect : SoundVector) {
        Mix_FreeChunk(soundEffect);
    }
    Mix_FreeMusic(music);
    Mix_CloseAudio();
    Mix_Quit();
}
