//
// Created by romak on 17.01.2025.
//

#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include "Setup.h"
#include <algorithm>
#include <io.h>
#include <iostream>
#include <ostream>
#include <unordered_map>
#include <vector>

struct AnimationVector {
  int frameNumber;
  float frameDelay, expiration;
  int angle, AnimationNumber;
  SDL_FRect destRect;
  uint32_t EUID;
};

class TextureManager {
public:
  static std::vector<SDL_Texture *> TextureVec;
  static std::vector<AnimationVector> animationsVec;
  static std::vector<IMG_Animation *> ImgAnimationsVec;
  static void Init();
  static SDL_Texture *LoadTexture(const char *filepath);
  static IMG_Animation *LoadAnimation(const char *filepath);
  static void AnimationCleaning();
  static void DrawAnimation(AnimationVector &animationVector, bool colorMod,
                            SDL_Renderer *renderer);
  static void DrawTexture(SDL_Texture *texture, SDL_Renderer *renderer,
                          SDL_FRect *src, SDL_FRect *dst, int angle);
  static void DrawTextureNP(int TextureNumber, SDL_Renderer *renderer,
                            SDL_FRect *dst, int angle);
  static void DrawMap(SDL_Texture *texture, SDL_Renderer *renderer,
                      SDL_FRect *src, SDL_FRect *dst);
};

#endif // TEXTUREMANAGER_H
