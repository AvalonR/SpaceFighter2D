//
// Created by romak on 16.01.2025.
//

#ifndef PLAYER_H
#define PLAYER_H

#include "Setup.h"
#include <string>

struct Achieve {
    bool achieved;
    std::string title, description;
};
struct PlayerUpgradeStats {
    float ExperienceP = 0;
    int extraBulletsPerShot = 0;
    bool firemodes[3]; // 0 -> shotgun spread | 1 -> burst fire | 2 -> parallel fire
    float bonusTotalHP = 0;
    float PassiveHealing = 1.0f;
    float MovementSpeed = 1.0f;
    float extraBulletCap = 0;
    float damagePerBullet = 0;
    float bulletCooldownMultiplier = 1.0f; // < 1.0 = faster
};

class Player {
    public:
    static bool LeftMouse, RightMouse;
    static int enemies_killed;
    static int bosses_killed;
    static int shots_landed;
    static int coin_pickup_played;
    static bool finished_level;
    static bool upgradedPlayer;
    static bool hitPlayerWave;
    static bool hitPlayerLevel;
    static PlayerUpgradeStats PlayerUpgrades;
    static Achieve Achievements[15];
    Player();
    ~Player();
    float HP{}, BC{};
    float x, y;
    SDL_FRect srcR{}, dstR{};
    int TextureID{};
    Vector velocity{};
    int rotation{}, shooting_delay{};
    static void playerInput();
    static void AchievementLogging(); 
};



#endif //PLAYER_H
