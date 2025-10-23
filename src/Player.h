//
// Created by romak on 16.01.2025.
//

#ifndef PLAYER_H
#define PLAYER_H

#include "Setup.h"
#include <string>
#include <unordered_map>

struct Achieve {
  bool achieved;
  std::string title, description;
};
struct PlayerUpgradeStats {
  float ExperienceP = 0;
  int extraBulletsPerShot = 0;
  bool firemodes[3]; // 0 -> shotgun spread | 1 -> burst fire | 2 -> parallel
                     // fire
  float TotalHP = 1.0f;
  float PassiveHealing = 1.0f;
  float MovementSpeed = 1.0f;
  float TotalBulletCap = 1.0f;
  float damagePerBullet = 0;
  float bulletCooldownMultiplier = 1.0f; // < 1.0 = faster
};

class Player {
public:
  static std::unordered_map<SDL_Keycode, bool>
      keyState; // Holds the state of keys
  static bool LeftMouse, RightMouse;
  static int enemies_killed;
  static int bosses_killed;
  static int shots_landed;
  static int coin_pickup_played;
  static bool finished_level;
  static bool upgradedPlayer;
  static bool hitPlayerWave;
  static bool hitPlayerLevel;
  static bool levelUpSoundPlayed;
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
  static void playerInput(GameManager &gm);
  static void AchievementLogging(GameManager &gm);
};

#endif // PLAYER_H
