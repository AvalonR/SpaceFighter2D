//
// Created by romak on 18.01.2025.
//

#ifndef ENEMY_H
#define ENEMY_H

#include "Setup.h"
struct Rotation_ID {
  int rotation;
  uint32_t UID;
};
struct EnemyWave {
  int EnemyTypes[2];         // Types of enemies in this wave
  int EnemyCount[2];         // How many of each type
  float delayBetweenWaves{}; // Countdown before next wave starts
  bool activated{};          // If the wave has started
};
class Enemy {
public:
  Enemy();
  ~Enemy();
  float HP{}, BC{};
  float x{}, y{};
  SDL_FRect srcR{}, dstR{};
  int TextureID{};
  Vector velocity{};
  int rotation{}, shooting_delay{}, type{};
  static float basicDelay[4];
  static std::vector<EnemyWave> Waves;
  static std::vector<Rotation_ID> enemyRotations;
  static void RestartWaves();
  static int Randomizer(int x, int y);
  static void SwarmingScenario();
  static void SpawnEnemy(int type);
  static EntityStats toEntityStats(Enemy enemy, uint32_t UID);
  static void EnemyAI(GameManager &gm);
};

#endif // ENEMY_H
