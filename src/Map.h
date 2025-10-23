//
// Created by romak on 23.01.2025.
//
#pragma once

#ifndef MAP_H
#define MAP_H

#include "Setup.h"
#include <vector>

class Map {
public:
    static std::vector<const char*> MapTextureNames;
    static SDL_Texture* MapTexture;
    static SDL_FRect* src, *dst;

    static float lerp(float start, float end, float t);
    static void NewMap(int levelID);
    static void MapGeneration();
    static void MapUpdate(GameManager& gm);
    static void MapRender(GameManager& gm);
};



#endif //MAP_H
