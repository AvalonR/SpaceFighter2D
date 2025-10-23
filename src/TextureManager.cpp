//
// Created by romak on 17.01.2025.
//

#include "TextureManager.h"
#include "Bullet.h"
#include "Setup.h"

#include <unordered_map>

std::vector<SDL_Texture *> TextureManager::TextureVec;
std::vector<AnimationVector> TextureManager::animationsVec;
std::vector<IMG_Animation *> TextureManager::ImgAnimationsVec;

void TextureManager::Init() {

  if (!Bullet::textureBullet) {
    Bullet::textureBullet = TextureManager::LoadTexture("laser.png");
  }
  // Loading animations
  IMG_Animation *explosion = TextureManager::LoadAnimation("explosion.gif");
  IMG_Animation *deathAnimationSuicide =
      TextureManager::LoadAnimation("torpedodest.gif");
  IMG_Animation *deathAnimationBomber =
      TextureManager::LoadAnimation("bomberdest.gif");
  IMG_Animation *deathAnimationDreadnought =
      TextureManager::LoadAnimation("dreadnoughtdest.gif");
  IMG_Animation *fullbeam = TextureManager::LoadAnimation("fullbeam.gif");
  IMG_Animation *dreadnoughtweapon =
      TextureManager::LoadAnimation("dreadnoughtlaser.gif");
  IMG_Animation *deathAnimationBattlecruiser =
      TextureManager::LoadAnimation("NairanBattlecruiserDestruction.gif");
  IMG_Animation *battlecruiserweapon =
      TextureManager::LoadAnimation("NairanBattlecruiserWeapons.gif");
  IMG_Animation *enginesPlayer =
      TextureManager::LoadAnimation("enginesPlayer.gif");
  IMG_Animation *enginesBomber =
      TextureManager::LoadAnimation("enginesBomber.gif");
  IMG_Animation *enginesTorpedo =
      TextureManager::LoadAnimation("enginesTorpedo.gif");
  IMG_Animation *enginesDreadnought =
      TextureManager::LoadAnimation("enginesDreadnought.gif");
  IMG_Animation *enginesBattlecruiser =
      TextureManager::LoadAnimation("enginesBattlecruiser.gif");
  TextureManager::ImgAnimationsVec.emplace_back(deathAnimationSuicide);
  TextureManager::ImgAnimationsVec.emplace_back(explosion);
  TextureManager::ImgAnimationsVec.emplace_back(deathAnimationBomber);
  TextureManager::ImgAnimationsVec.emplace_back(deathAnimationDreadnought);
  TextureManager::ImgAnimationsVec.emplace_back(fullbeam);
  TextureManager::ImgAnimationsVec.emplace_back(dreadnoughtweapon);
  TextureManager::ImgAnimationsVec.emplace_back(deathAnimationBattlecruiser);
  TextureManager::ImgAnimationsVec.emplace_back(battlecruiserweapon);
  TextureManager::ImgAnimationsVec.emplace_back(enginesPlayer);
  TextureManager::ImgAnimationsVec.emplace_back(enginesBomber);
  TextureManager::ImgAnimationsVec.emplace_back(enginesTorpedo);
  TextureManager::ImgAnimationsVec.emplace_back(enginesDreadnought);
  TextureManager::ImgAnimationsVec.emplace_back(enginesBattlecruiser);
  // Loading ship textures
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("trimmed_playership.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("trimmed_bomber.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("trimmed_torpedo.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("tdreadnought.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("tBattlecruiser.png"));
  // Loading icons
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/Skillicon_01.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/Skillicon_02.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/Skillicon_05.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/Skillicon_07.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/Skillicon_11.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/Skillicon_13.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/Skillicon_14.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/Skillicon_16.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/Skillicon_17.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/Skillicon_19.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/Skillicon_21.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/Skillicon_22.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/Skillicon_23.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/Skillicon_25.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/Skillicon_30.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/Skillicon_32.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/Skillicon_33.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/Skillicon_36.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/Skillicon_27.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/Skillicon_34.png")); // element -> 24
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/keyboard_e.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/keyboard_e_outline.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/keyboard_q.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/keyboard_q_outline.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/keyboard_escape.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/keyboard_escape_outline.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/keyboard_w.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/keyboard_w_outline.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/keyboard_s.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/keyboard_s_outline.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/keyboard_a.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/keyboard_a_outline.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/keyboard_d.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/keyboard_d_outline.png"));
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/mouse_move.png")); // element -> 39
  TextureManager::TextureVec.emplace_back(
      TextureManager::LoadTexture("icons/Skillicon_06.png")); // element -> 40
}
SDL_Texture *TextureManager::LoadTexture(const char *filename) {
  std::string filenamestr(filename);
  std::string assetspath = Setup::basePath + "assets/" + filenamestr;
  std::cout << assetspath << std::endl;
  SDL_Surface *loadedSurface = IMG_Load(assetspath.c_str());
  if (loadedSurface == nullptr) {
    std::cout << SDL_GetError() << std::endl;
  }
  if (!loadedSurface) {
    std::cerr << "Failed to load surface: " << SDL_GetError() << std::endl;
    return nullptr;
  }
  SDL_Texture *texture = SDL_CreateTextureFromSurface(
      GameManagerSingleton::instance().getRenderer(), loadedSurface);
  SDL_DestroySurface(loadedSurface);
  if (!texture) {
    std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
  }
  return texture;
}

IMG_Animation *TextureManager::LoadAnimation(const char *filename) {
  std::string filenamestr(filename);
  std::string assetspath = Setup::basePath + "assets/" + filenamestr;
  std::cout << assetspath << std::endl;
  IMG_Animation *anim = IMG_LoadAnimation(assetspath.c_str());
  if (!anim) {
    std::cerr << "Failed to load animation: " << SDL_GetError() << std::endl;
  }
  return anim;
}
void TextureManager::AnimationCleaning() {
  animationsVec.erase(std::remove_if(animationsVec.begin(), animationsVec.end(),
                                     [](AnimationVector &anim) {
                                       return anim.expiration <= 0.0f;
                                     }),
                      animationsVec.end());
}
void TextureManager::DrawAnimation(AnimationVector &animationVector,
                                   bool colorMod, SDL_Renderer *renderer) {
  if (animationVector.AnimationNumber < 0 ||
      animationVector.AnimationNumber >= ImgAnimationsVec.size()) {
    std::cerr << "Invalid AnimationNumber: " << animationVector.AnimationNumber
              << std::endl;
    return;
  }
  if (animationVector.expiration >= 0) {
    if (!GameManagerSingleton::instance().get_is_Paused()) {
      animationVector.expiration -= GameManagerSingleton::instance().getDelta();
      if (animationVector.expiration <= 0.0f)
        animationVector.expiration = 0.0f;
    }
    // Handle frame delay
    if (animationVector.frameDelay <= 0.0f &&
        !GameManagerSingleton::instance().get_is_Paused()) {
      animationVector.frameDelay = 20.0f;
      if (animationVector.frameNumber >=
              ImgAnimationsVec[animationVector.AnimationNumber]->count ||
          animationVector.frameNumber + 1 >=
              ImgAnimationsVec[animationVector.AnimationNumber]->count) {
        animationVector.frameNumber = 0; // Loop animation
      }
      if (!GameManagerSingleton::instance().get_is_Paused()) {
        animationVector.frameNumber++;
      }
    } else {
      if (!GameManagerSingleton::instance().get_is_Paused()) {
        animationVector.frameDelay -=
            GameManagerSingleton::instance().getDelta();
      }
    }
    if (ImgAnimationsVec[animationVector.AnimationNumber]
            ->frames[animationVector.frameNumber] != nullptr) {
      SDL_Texture *texture = SDL_CreateTextureFromSurface(
          renderer, ImgAnimationsVec[animationVector.AnimationNumber]
                        ->frames[animationVector.frameNumber]);
      if (colorMod) {
        SDL_SetTextureColorMod(texture, 255, 100, 100);
      }
      if (animationVector.AnimationNumber == 4) {
        SDL_FPoint rotationCenter = {animationVector.destRect.w / 2, 0};
        SDL_RenderTextureRotated(
            renderer, texture, nullptr, &animationVector.destRect,
            animationVector.angle, &rotationCenter, SDL_FLIP_NONE);
      } else {
        SDL_RenderTextureRotated(renderer, texture, nullptr,
                                 &animationVector.destRect,
                                 animationVector.angle, nullptr, SDL_FLIP_NONE);
      }
      SDL_DestroyTexture(texture);
    }
  } else {
    return; // Stop if animation is expired
  }
}

void TextureManager::DrawTextureNP(int TextureNumber, SDL_Renderer *renderer,
                                   SDL_FRect *dst, int angle) {
  SDL_RenderTextureRotated(renderer, TextureVec[TextureNumber], nullptr, dst,
                           angle, nullptr, SDL_FLIP_NONE);
}
void TextureManager::DrawTexture(SDL_Texture *texture, SDL_Renderer *renderer,
                                 SDL_FRect *src, SDL_FRect *dst, int angle) {
  SDL_RenderTextureRotated(renderer, texture, nullptr, dst, angle, nullptr,
                           SDL_FLIP_NONE);
}
void TextureManager::DrawMap(SDL_Texture *texture, SDL_Renderer *renderer,
                             SDL_FRect *src, SDL_FRect *dst) {
  SDL_RenderTexture(renderer, texture, src, dst);
}
