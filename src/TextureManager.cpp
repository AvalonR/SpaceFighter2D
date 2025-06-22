//
// Created by romak on 17.01.2025.
//

#include "TextureManager.h"

#include <unordered_map>

std::vector<SDL_Texture*> TextureManager::TextureVec;
std::vector<AnimationVector> TextureManager::animationsVec;
std::vector<IMG_Animation*> TextureManager::ImgAnimationsVec;

SDL_Texture* TextureManager::LoadTexture(const char* filename)
{
    std::string filenamestr(filename);
    std::string assetspath = Setup::basePath + "assets/" + filenamestr;
    std::cout << assetspath << std::endl;
    SDL_Surface* loadedSurface = IMG_Load(assetspath.c_str());
    if (loadedSurface == nullptr)
    {
        std::cout << SDL_GetError() << std::endl;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(Setup::renderer, loadedSurface);
    return texture;
}

IMG_Animation* TextureManager::LoadAnimation(const char* filename)
{
    std::string filenamestr(filename);
    std::string assetspath = Setup::basePath + "assets/" + filenamestr;
    std::cout << assetspath << std::endl;
    if (!IMG_LoadAnimation(assetspath.c_str()))
    {
        std::cout << SDL_GetError() << std::endl;
    }
    return IMG_LoadAnimation(assetspath.c_str());
}
void TextureManager::AnimationCleaning()
{
    animationsVec.erase(
        std::remove_if(animationsVec.begin(), animationsVec.end(),
                       [](AnimationVector& anim) { return anim.expiration < 0; }),
        animationsVec.end());
}
void TextureManager::DrawAnimation(AnimationVector& animationVector, bool colorMod)
{    
    if (animationVector.AnimationNumber < 0 || animationVector.AnimationNumber >= ImgAnimationsVec.size()) {
        std::cerr << "Invalid AnimationNumber: " << animationVector.AnimationNumber << std::endl;
        return;
    }
    if (animationVector.expiration >= 0)
    {
        if (!Setup::is_Paused)
        {
            animationVector.expiration--; // Reduce expiration time
        }
        // Handle frame delay
        if (animationVector.frameDelay == 0 && !Setup::is_Paused)
        {
            animationVector.frameDelay = 20;
            if (animationVector.frameNumber >= ImgAnimationsVec[animationVector.AnimationNumber]->count || animationVector.frameNumber + 1 >= ImgAnimationsVec[animationVector.AnimationNumber]->count)
            {
                animationVector.frameNumber = 0; // Loop animation
            }
            if (!Setup::is_Paused) {
                animationVector.frameNumber++;
            }
        }
        else
        {
            if (!Setup::is_Paused) {
                animationVector.frameDelay--;
            }
        }
        if (ImgAnimationsVec[animationVector.AnimationNumber]->frames[animationVector.frameNumber] != nullptr) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(Setup::renderer, ImgAnimationsVec[animationVector.AnimationNumber]->frames[animationVector.frameNumber]);
            if (colorMod) {
                SDL_SetTextureColorMod(texture, 255, 100, 100);
            }   
            if (animationVector.AnimationNumber == 4) {
                SDL_FPoint rotationCenter = {animationVector.destRect.w / 2, 0};
                SDL_RenderTextureRotated(Setup::renderer, texture, nullptr, &animationVector.destRect, animationVector.angle, &rotationCenter, SDL_FLIP_NONE);
            } else {
                SDL_RenderTextureRotated(Setup::renderer, texture, nullptr, &animationVector.destRect, animationVector.angle, nullptr, SDL_FLIP_NONE);
            }
            SDL_DestroyTexture(texture);
        }
    }
    else
    {
        return; // Stop if animation is expired
    }
}

void TextureManager::DrawTextureNP(int TextureNumber, SDL_Renderer* renderer, SDL_FRect* dst, int angle)
{
    SDL_RenderTextureRotated(renderer, TextureVec[TextureNumber], nullptr, dst, angle, nullptr, SDL_FLIP_NONE);
}
void TextureManager::DrawTexture(SDL_Texture* texture, SDL_Renderer* renderer, SDL_FRect* src, SDL_FRect* dst, int angle)
{
    SDL_RenderTextureRotated(renderer, texture, nullptr, dst, angle, nullptr, SDL_FLIP_NONE);
}
void TextureManager::DrawMap(SDL_Texture* texture, SDL_Renderer* renderer, SDL_FRect* src, SDL_FRect* dst)
{
    SDL_RenderTexture(renderer, texture, src, dst);
}
