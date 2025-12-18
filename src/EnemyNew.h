#ifndef ENEMY_NEW_H
#define ENEMY_NEW_H

#include "Entity.h"
#include "Setup.h"
#include <vector>

struct Rotation_ID {
  int rotation;
  uint32_t UID;
};

struct EnemyWave {
  int EnemyTypes[2];
  int EnemyCount[2];
  float delayBetweenWaves{};
  bool activated{};
};

class Enemy : public Entity {
public:
  static int Randomizer(int min, int max);
  Enemy();
  virtual ~Enemy() = default;

  void update(GameManager &gm) override;
  void onDeath(GameManager &gm) override;

  virtual void updateAI(GameManager &gm, Entity &player, double deltaTime) = 0;

  virtual int getScoreValue() const = 0;
  virtual int getCoinDrop() const = 0;
  virtual float getExperienceDrop() const = 0;

  static float basicDelay[4];
  static std::vector<EnemyWave> Waves;
  static std::vector<Rotation_ID> enemyRotations;
  static void RestartWaves();
  static void SwarmingScenario();
  static Entity *SpawnEnemy(int type);
};

class BasicEnemy : public Enemy {
private:
  float orbitDirection;

public:
  BasicEnemy();
  void updateAI(GameManager &gm, Entity &player, double deltaTime) override;
  int getTypeID() const override { return 1; }
  int getScoreValue() const override { return 20; }
  int getCoinDrop() const override { return 10; }
  float getExperienceDrop() const override { return 0.15f; }

  void setOrbitDirection(float dir) { orbitDirection = dir; }
};

class TorpedoEnemy : public Enemy {
public:
  TorpedoEnemy();
  void updateAI(GameManager &gm, Entity &player, double deltaTime) override;
  void onDeath(GameManager &gm) override;
  int getTypeID() const override { return 2; }
  int getScoreValue() const override { return 25; }
  int getCoinDrop() const override { return 15; }
  float getExperienceDrop() const override { return 0.1f; }
};

class DreadnoughtEnemy : public Enemy {
private:
  void updateBeamWeapon(GameManager &gm);

public:
  DreadnoughtEnemy();
  void updateAI(GameManager &gm, Entity &player, double deltaTime) override;
  int getTypeID() const override { return 3; }
  int getScoreValue() const override { return 250; }
  int getCoinDrop() const override { return 100; }
  float getExperienceDrop() const override { return 0.3f; }
};

class BattlecruiserEnemy : public Enemy {
private:
  float orbitDirection;

public:
  BattlecruiserEnemy();
  void updateAI(GameManager &gm, Entity &player, double deltaTime) override;
  int getTypeID() const override { return 4; }
  int getScoreValue() const override { return 350; }
  int getCoinDrop() const override { return 150; }
  float getExperienceDrop() const override { return 0.3f; }

  void setOrbitDirection(float dir) { orbitDirection = dir; }
};

#endif // ENEMY_NEW_H
