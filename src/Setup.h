//
// Created by romak on 16.01.2025.
//

#ifndef SETUP_H
#define SETUP_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <string>
#include <vector>

#include "Map.h"

struct Vector
{
    float x, y;
};

struct EntityStats
{
    float HP, BC, type;
    float x, y;
    int TextureID{};
    SDL_FRect src, dest;
    double rotation;
    Vector velocity;
    int shooting_delay, effectTimer;
};

class Setup
{
public:
    static int Score;
    static int Coins;
    static int TargetCoins;
    static int TargetScore;
    static int CurrentLevel;
    static float speed;
    static float Difficulty;
    static std::string basePath;
    static std::vector<EntityStats> EntityList;
    static std::vector<int> ScoreHistory;
    static bool Vsync;
    static bool loopRunning;
    static bool is_Paused;
    static bool Restart;
    static float Max_FPS;
    static SDL_Event event;
    static SDL_Window* window;
    static int WindowHeight, WindowWidth;
    static SDL_Renderer* renderer;
    static Vector mouseCoordin;
    static void initialization();
    static void gameLoop();
    static void restart();
    static void nextLevel();
    static void render();
    static void update();
    static void quit();
    static void ScoreChange();
    static void CoinsChange();
};


#endif //SETUP_H
