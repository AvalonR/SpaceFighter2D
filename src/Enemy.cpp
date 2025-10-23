//
// Created by romak on 18.01.2025.
//

#include "Enemy.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <random>

#include "Bullet.h"
#include "Loot.h"
#include "Player.h"
#include "Setup.h"
#include "Sound.h"
#include "TextureManager.h"
#include "UI.h"
float angle = 0.0f;    // Angle in radians, used for circular movement
float radius = 400.0f; // Distance from the player
float rotationSpeed = 0.1f;
std::vector<Rotation_ID> Enemy::enemyRotations;

Enemy::Enemy() {
  HP = 1.0f;
  BC = 1.0f;
  x = y = 0.0;
  srcR = {};
  dstR = {0, 0, 64, 60};
  TextureID = 1;
  velocity.x = velocity.y = 0;
  rotation = 0;
  shooting_delay = 0;
  velocity.x = velocity.y = 0;
  type = 0;
}

Enemy::~Enemy() = default;

float Enemy::basicDelay[4] = {3000, 4000, 5000, 6000};
std::vector<EnemyWave> Enemy::Waves;

int currentWaveIndex = 0; // Tracks which wave we're on
void Enemy::RestartWaves() {
  currentWaveIndex = 0;
  for (int t = 0; t < Waves.size(); t++) {
    Waves[t].activated = false;
    Waves[t].delayBetweenWaves = basicDelay[t];
  }
}
int aclock = 0;

void Enemy::SwarmingScenario() {
  if (currentWaveIndex >= Waves.size()) {
    if (GameManagerSingleton::instance()
            .getEntityManager()
            .getEntities()
            .size() == 1) { // only player alive
      Player::finished_level = true;
      if (aclock > 500) {
        aclock = 0;
        Setup::nextLevel(GameManagerSingleton::instance());
      } else {
        aclock++;
      }
    }
    return;
  }
  EnemyWave &currentWave = Waves[currentWaveIndex];

  if (!currentWave.activated) {
    for (int t = 0; t < 2; t++) {
      if (currentWave.EnemyCount[t] > 0 && currentWave.EnemyTypes[t] > 0) {
        for (int j = 0; j < currentWave.EnemyCount[t]; j++) {
          SpawnEnemy(currentWave.EnemyTypes[t]);
        }
      }
    }
    currentWave.activated = true;
  } else {
    if (currentWave.delayBetweenWaves >= 0.0f) {
      currentWave.delayBetweenWaves -=
          GameManagerSingleton::instance().getDelta();
    } else {
      Player::hitPlayerWave = false;
      currentWaveIndex++;
    }
  }
}

static constexpr Vector SpawnPositions[] = {
    {-400, -400}, {960, -400},  {2320, -400}, {-400, 1280},
    {960, 1580},  {2320, 1280}, {-400, 540},  {2320, 540}};

int Enemy::Randomizer(int x, int y) {
  std::random_device rd;
  std::uniform_int_distribution<int> distrib(x, y);
  return distrib(rd);
}
EntityStats Enemy::toEntityStats(Enemy enemy, uint32_t UID) {
  EntityStats stats;
  stats.HP = enemy.HP;
  stats.BC = enemy.BC;
  stats.type = enemy.type;
  stats.x = enemy.x;
  stats.y = enemy.y;
  stats.TextureID = enemy.TextureID;
  stats.src = enemy.srcR;
  stats.dest = enemy.dstR;
  stats.rotation = enemy.rotation;
  stats.velocity = enemy.velocity;
  stats.shooting_delay = enemy.shooting_delay;
  stats.effectTimer = 0;
  stats.UID = UID;
  return stats;
}

void Enemy::SpawnEnemy(int type) {
  Enemy enemy;
  enemy.x = SpawnPositions[Randomizer(0, 7)].x;
  enemy.y = SpawnPositions[Randomizer(0, 7)].y;
  if (enemy.x > 0 &&
      enemy.x <
          static_cast<float>(GameManagerSingleton::instance().getWindowW()) &&
      enemy.y > 0 &&
      enemy.y <
          static_cast<float>(GameManagerSingleton::instance().getWindowH())) {
    // Force the spawn position to be off-screen
    if (Randomizer(0, 1) == 0)
      enemy.x = (Randomizer(0, 1) == 0)
                    ? -200
                    : GameManagerSingleton::instance().getWindowW() + 200;
    else
      enemy.y = (Randomizer(0, 1) == 0)
                    ? -200
                    : GameManagerSingleton::instance().getWindowH() + 200;
  }

  enemy.dstR.x = enemy.x;
  enemy.dstR.y = enemy.y;
  enemy.dstR.h = enemy.dstR.h = 64;
  if (type == 1) {
    enemy.dstR.w = 64;
    enemy.dstR.h = 60;
  }
  if (type == 2) {
    enemy.dstR.w = 84;
    enemy.dstR.h = 58;
  }
  if (type == 3) {
    enemy.HP = 3.0f;
    enemy.dstR.w = 68 * 2;
    enemy.dstR.h = 98 * 2;
    enemy.rotation = 180;
    enemy.shooting_delay = 1200;
  }
  if (type == 4) {
    enemy.HP = 6.0f;
    enemy.dstR.w = 62 * 2;
    enemy.dstR.h = 85 * 2;
    enemy.shooting_delay = 700;
  }
  enemy.type = type;
  enemy.TextureID = type;

  uint32_t uid =
      GameManagerSingleton::instance().getEntityManager().generateID(type);
  GameManagerSingleton::instance().getEntityManager().add(
      toEntityStats(enemy, uid));
  if (type == 1) {
    enemyRotations.emplace_back((Randomizer(-2, 2)), uid);
  } else if (type == 4) {
    enemyRotations.emplace_back((Randomizer(-1, 1)), uid);
  }
}

void Enemy::EnemyAI(GameManager &gm) {
  gm.getEntityManager().removeDeadEntities(gm);
  for (auto &Entity : gm.getEntityManager().getEntities()) {
    float forceX = 0.0f, forceY = 0.0f;

    if (Entity.type == 1) {
      // Compute attraction to player
      float dx = gm.getEntityManager().getPlayer().dest.x - Entity.x;
      float dy = gm.getEntityManager().getPlayer().dest.y - Entity.y;
      float distance = std::sqrt(dx * dx + dy * dy);

      if (distance > radius) {
        forceX += (dx / distance) * 0.5f;
        forceY += (dy / distance) * 0.5f;
      }

      // Repulsion from other enemies
      for (auto &OtherEnemy : gm.getEntityManager().getEntities()) {
        if (Entity.UID == OtherEnemy.UID)
          continue;

        float diffX = Entity.x - OtherEnemy.x;
        float diffY = Entity.y - OtherEnemy.y;
        float dist = std::sqrt(diffX * diffX + diffY * diffY);

        if (dist < 100 && dist > 40) // Smooth repulsion
        {
          float repelForce = 0.3f / dist;
          forceX += (diffX / dist) * repelForce;
          forceY += (diffY / dist) * repelForce;
        } else if (dist <= 40) {
          // Soft randomization to prevent stacking
          forceX += -1.5f * static_cast<float>(Randomizer(-1, 1));
          forceY += -1.5f * static_cast<float>(Randomizer(-1, 1));
        }
      }
      float orbitDirection;
      for (auto &rotations : enemyRotations) {
        if (rotations.UID == Entity.UID) {
          orbitDirection = static_cast<float>(rotations.rotation);
          break;
        }
      }

      float angle = std::atan2(dy, dx);
      float orbitForceX = std::sin(angle) * 0.5f * orbitDirection;
      float orbitForceY = -std::cos(angle) * 0.5f * orbitDirection;

      forceX += orbitForceX;
      forceY += orbitForceY;

      // Enemy Shooting
      Entity.rotation = (std::atan2(dy, dx) * (180.0f / M_PI)) + 90;

      if (Entity.shooting_delay <= 0.0f) {
        Bullet::spawnBulletEnemy({Entity.dest.x, Entity.dest.y},
                                 {Entity.dest.w, Entity.dest.h},
                                 Entity.rotation, Entity.UID);
        Entity.shooting_delay = 200;
      } else {
        Entity.shooting_delay -= gm.getDelta();
      }

      // Apply forces using smooth interpolation
      Entity.velocity.x = Map::lerp(Entity.velocity.x, forceX, 0.1f);
      Entity.velocity.y = Map::lerp(Entity.velocity.y, forceY, 0.1f);
    }

    if (Entity.type == 2) {
      float dx = gm.getEntityManager().getPlayer().dest.x - Entity.x;
      float dy = gm.getEntityManager().getPlayer().dest.y - Entity.y;
      float distance = std::sqrt(dx * dx + dy * dy);

      if (distance > 20) // Faster approach
      {
        forceX += (dx / distance) * 0.7f;
        forceY += (dy / distance) * 0.7f;
      }

      Entity.rotation = std::atan2(dy, dx) * (180.0f / M_PI) + 90;

      // Explosion on contact
      if (SDL_HasRectIntersectionFloat(&gm.getEntityManager().getPlayer().dest,
                                       &Entity.dest)) {
        SDL_FRect result;
        SDL_GetRectIntersectionFloat(&gm.getEntityManager().getPlayer().dest,
                                     &Entity.dest, &result);

        result.w = std::max(Entity.dest.w, result.w);
        result.h = std::max(Entity.dest.h, result.h);

        TextureManager::animationsVec.emplace_back(0, 20, 200, 0, 1, result,
                                                   Entity.UID);
        Entity.HP = -0.9999f;
        gm.getEntityManager().getPlayer().HP -=
            (0.2f * GameManagerSingleton::instance().getDifficulty());
        gm.getEntityManager().getPlayer().effectTimer = 40;
        Player::hitPlayerLevel = true;
        Player::hitPlayerWave = true;
        float angle =
            std::atan2(Entity.dest.y - result.y, Entity.dest.x - result.x);
        gm.getEntityManager().getPlayer().velocity.x =
            Map::lerp(gm.getEntityManager().getPlayer().velocity.x,
                      gm.getEntityManager().getPlayer().velocity.x -
                          std::cos(angle) * 0.4,
                      0.1f); // Knockback effect
        gm.getEntityManager().getPlayer().velocity.y =
            Map::lerp(gm.getEntityManager().getPlayer().velocity.y,
                      gm.getEntityManager().getPlayer().velocity.y -
                          std::sin(angle) * 0.4,
                      0.1f); // Knockback effect
        Sound::PlaySound(2);
      }

      // Repulsion from other enemies
      for (auto &OtherEnemy : gm.getEntityManager().getEntities()) {
        if (Entity.UID == OtherEnemy.UID)
          continue;

        float diffX = Entity.x - OtherEnemy.x;
        float diffY = Entity.y - OtherEnemy.y;
        float dist = std::sqrt(diffX * diffX + diffY * diffY);

        if (dist < 100 && dist > 40) {
          float repelForce = 0.3f / dist;
          forceX += (diffX / dist) * repelForce;
          forceY += (diffY / dist) * repelForce;
        } else if (dist <= 40) {
          forceX += -1.5f * static_cast<float>(Randomizer(-1, 1));
          forceY += -1.5f * static_cast<float>(Randomizer(-1, 1));
        }
      }

      // Smooth movement
      Entity.velocity.x = Map::lerp(Entity.velocity.x, forceX, 0.2f);
      Entity.velocity.y = Map::lerp(Entity.velocity.y, forceY, 0.2f);
    }
    if (Entity.type == 3) {
      float forceX = 0.0f, forceY = 0.0f;
      float dy = 0;
      float dx = gm.getEntityManager().getPlayer().dest.x - Entity.x;
      Entity.y = Map::lerp(Entity.y, 100.0f, 0.02f);
      if (Entity.y <= 60 || Entity.y >= 140) {
        dy = 100.0f - Entity.y;
      }

      float distance = std::sqrt(dx * dx + dy * dy);

      forceX += (dx / distance) * 0.7f;
      forceY += (dy / distance) * 0.7f;
      dx = gm.getEntityManager().getPlayer().x - Entity.x;
      dy = gm.getEntityManager().getPlayer().y - Entity.y;
      float current = Entity.rotation;
      float target =
          std::fmod(std::atan2(dy, dx) * (180.0f / M_PI) + 450.0f, 360.0f);
      float delta = std::fmod((target - current + 540.0f), 360.0f) - 180.0f;
      Entity.rotation += delta * 0.02f;
      if (Entity.shooting_delay <= 450.0f && Entity.shooting_delay >= 350.0f) {
        SDL_FRect shipanim = {Entity.dest.x - Entity.dest.w / 2,
                              Entity.dest.y - Entity.dest.h / 2, 256, 256};
        TextureManager::animationsVec.emplace_back(0, 20, 800, 180, 5, shipanim,
                                                   Entity.UID);
        Sound::PlaySound(7);
      }
      if (Entity.shooting_delay <= 0.0f) {
        SDL_FRect a = {Entity.x + Entity.dest.w / 2 - 128,
                       Entity.y + Entity.dest.h, 256, 1};
        TextureManager::animationsVec.emplace_back(
            0, 1, 450, Entity.rotation - 180, 4, a, Entity.UID);
        Sound::PlaySound(8);
        Entity.shooting_delay = 1200;
      }
      for (int t = 0; t < TextureManager::animationsVec.size(); t++) {
        if (TextureManager::animationsVec[t].EUID == Entity.UID) {
          if (TextureManager::animationsVec[t].AnimationNumber == 4) {
            TextureManager::animationsVec[t].angle = Entity.rotation - 180;
            float angleRad = (Entity.rotation - 90.0f) * (M_PI / 180.0f);
            float beamOffset = 100.0f;

            float cosA = std::cos(angleRad);
            float sinA = std::sin(angleRad);
            float HalfWidth = TextureManager::animationsVec[t].destRect.w / 2;
            float Height = TextureManager::animationsVec[t].destRect.h;

            float originX = Entity.x + Entity.dest.w / 2 + cosA * beamOffset;
            float originY = Entity.y + Entity.dest.h / 2 + sinA * beamOffset;

            angleRad = angleRad + M_PI / 7.0f;
            cosA = std::cos(angleRad - 90);
            sinA = std::sin(angleRad - 90);

            TextureManager::animationsVec[t].destRect.x = originX - HalfWidth;
            TextureManager::animationsVec[t].destRect.y = originY;

            SDL_FPoint localCorners[6] = {
                {-HalfWidth / 2, 0.0f},   // Top-left
                {HalfWidth / 2, 0.0f},    // Top-right
                {HalfWidth / 2, Height},  // Bottom-right
                {-HalfWidth / 2, Height}, // Bottom-left
                {0.0f, 0.0f},             // Middle
                {0.0f, Height}            // Middle
            };

            SDL_FPoint Beamcorners[6];
            for (int i = 0; i < 6; ++i) {
              float lx = localCorners[i].x;
              float ly = localCorners[i].y;

              float rx = lx * cosA - ly * sinA;
              float ry = lx * sinA + ly * cosA;

              Beamcorners[i].x = originX + rx;
              Beamcorners[i].y = originY + ry;
            }

            // SDL_SetRenderDrawColor(Setup::renderer, 255, 200, 0, 200);
            // SDL_RenderLine(Setup::renderer, Beamcorners[0].x,
            // Beamcorners[0].y, Beamcorners[1].x, Beamcorners[1].y);
            // SDL_RenderLine(Setup::renderer, Beamcorners[0].x,
            // Beamcorners[0].y, Beamcorners[3].x, Beamcorners[3].y);
            // SDL_RenderLine(Setup::renderer, Beamcorners[1].x,
            // Beamcorners[1].y, Beamcorners[2].x, Beamcorners[2].y);
            // SDL_RenderLine(Setup::renderer, Beamcorners[3].x,
            // Beamcorners[3].y, Beamcorners[2].x, Beamcorners[2].y);
            // SDL_RenderLine(Setup::renderer, Beamcorners[3].x,
            // Beamcorners[3].y, Beamcorners[2].x, Beamcorners[2].y);
            //
            // SDL_SetRenderDrawColor(Setup::renderer, 255, 0, 0, 200);
            // SDL_RenderLine(Setup::renderer, Beamcorners[4].x,
            // Beamcorners[4].y, Beamcorners[5].x, Beamcorners[5].y);

            SDL_FPoint LinesForChecking[3][2] = {
                {Beamcorners[0].x, Beamcorners[0].y, Beamcorners[3].x,
                 Beamcorners[3].y},
                {Beamcorners[1].x, Beamcorners[1].y, Beamcorners[2].x,
                 Beamcorners[2].y},
                {Beamcorners[4].x, Beamcorners[4].y, Beamcorners[5].x,
                 Beamcorners[5].y},
            };

            bool hitSomething = false;
            int SoundCooldownLaser = 0;
            for (auto &enemy : gm.getEntityManager().getEntities()) {
              for (int i = 0; i < 3; i++) {
                if (enemy.type != 3 &&
                    SDL_GetRectAndLineIntersectionFloat(
                        &enemy.dest, &LinesForChecking[i][0].x,
                        &LinesForChecking[i][0].y, &LinesForChecking[i][1].x,
                        &LinesForChecking[i][1].y)) {
                  TextureManager::animationsVec[t].destRect.h =
                      Map::lerp(TextureManager::animationsVec[t].destRect.h,
                                LinesForChecking[i][1].y -
                                    TextureManager::animationsVec[t].destRect.y,
                                0.05f);

                  enemy.HP = Map::lerp(
                      enemy.HP,
                      enemy.HP -
                          (0.15f *
                           GameManagerSingleton::instance().getDifficulty()),
                      0.02f);
                  enemy.effectTimer = 15;
                  if (SoundCooldownLaser <= 0) {
                    Sound::PlaySound(12);
                    SoundCooldownLaser = 4;
                  } else {
                    SoundCooldownLaser--;
                  }
                  SDL_FPoint spark = {LinesForChecking[i][1].x,
                                      LinesForChecking[i][1].y};
                  Loot::ParticleCreation(spark, {255, 155, 0, 255}, 250, 2);
                  hitSomething = true;
                  break;
                }
              }
            }
            if (!hitSomething) {
              TextureManager::animationsVec[t].destRect.h = Map::lerp(
                  TextureManager::animationsVec[t].destRect.h, 748, 0.1f);
            }
          }

          if (TextureManager::animationsVec[t].AnimationNumber == 5 &&
              TextureManager::animationsVec[t].EUID == Entity.UID) {
            TextureManager::animationsVec[t].destRect.x =
                Entity.dest.x - Entity.dest.w / 2.3;
            TextureManager::animationsVec[t].destRect.y =
                Entity.dest.y - Entity.dest.h / 5.7;
            TextureManager::animationsVec[t].angle = Entity.rotation;
          }
        }
      }
      // Repulsion from other enemies
      for (auto &OtherEnemy : gm.getEntityManager().getEntities()) {
        if (Entity.UID == OtherEnemy.UID)
          continue;

        float diffX = Entity.x - OtherEnemy.x;
        float diffY = Entity.y - OtherEnemy.y;
        float dist = std::sqrt(diffX * diffX + diffY * diffY);

        if (dist < 500 && dist > 230) {
          float repelForce = 0.4f / dist;
          forceX += (diffX / dist) * repelForce * 4.0f;
        } else if (dist <= 230) {
          forceX += -1.5f * static_cast<float>(Randomizer(-2, 2));
        }
      }
      Entity.velocity.x = Map::lerp(Entity.velocity.x, forceX, 0.008f);
      Entity.velocity.y = Map::lerp(Entity.velocity.y, forceY, 0.04f);
      Entity.shooting_delay -= gm.getDelta();
    }

    if (Entity.type == 4) {
      float dx = gm.getEntityManager().getPlayer().dest.x - Entity.x;
      float dy = gm.getEntityManager().getPlayer().dest.y - Entity.y;
      float distance = std::sqrt(dx * dx + dy * dy);

      if (distance > radius + 100) {
        forceX += (dx / distance) * 0.5f;
        forceY += (dy / distance) * 0.5f;
      }

      for (auto &OtherEnemy : gm.getEntityManager().getEntities()) {
        if (Entity.UID == OtherEnemy.UID)
          continue;

        float diffX = Entity.x - OtherEnemy.x;
        float diffY = Entity.y - OtherEnemy.y;
        float dist = std::sqrt(diffX * diffX + diffY * diffY);

        if (dist < 100 && dist > 40) {
          float repelForce = 0.3f / dist;
          forceX += (diffX / dist) * repelForce;
          forceY += (diffY / dist) * repelForce;
        } else if (dist <= 40) {
          forceX += -1.5f * static_cast<float>(Randomizer(-1, 1));
          forceY += -1.5f * static_cast<float>(Randomizer(-1, 1));
        }
      }
      float orbitDirection;
      for (auto &rotations : enemyRotations) {
        if (rotations.UID == Entity.UID) {
          orbitDirection = static_cast<float>(rotations.rotation);
          break;
        }
      }

      float angle = std::atan2(dy, dx);
      float orbitForceX = std::sin(angle) * 0.5f * orbitDirection;
      float orbitForceY = -std::cos(angle) * 0.5f * orbitDirection;

      forceX += orbitForceX;
      forceY += orbitForceY;

      Entity.rotation = (std::atan2(dy, dx) * (180.0f / M_PI)) + 90;

      if (Entity.shooting_delay <= 0.0f) {
        SDL_FRect shipanim = {Entity.dest.x, Entity.dest.y, 256, 256};
        TextureManager::animationsVec.emplace_back(0, 20, 400, Entity.rotation,
                                                   7, shipanim, Entity.UID);
        float baseAngle = (Entity.rotation - 95) * (M_PI / 180.0f);
        float spread = M_PI / 8.0f;
        for (int i = 0; i < 12; i++) {
          float angle = baseAngle - spread / 2 + spread * (i / 9.0f);
          Bullet::SpawnBattlecruiserBullet(Entity.dest.x + Entity.dest.w / 2,
                                           Entity.dest.y + Entity.dest.h / 2,
                                           angle, angle, 0, 0.1f, Entity.UID);
        }
        Entity.shooting_delay = 450;
        Sound::PlaySound(15);
      } else {
        Entity.shooting_delay -= gm.getDelta();
      }

      for (auto &animation : TextureManager::animationsVec) {
        if (animation.AnimationNumber == 7 && animation.EUID == Entity.UID) {
          float centerX = Entity.dest.x + Entity.dest.w / 2.0f;
          float centerY = Entity.dest.y + Entity.dest.h / 2.0f;
          float offsetX = 0.0f;
          float offsetY = Entity.dest.h / 11.0f;

          float radians = Entity.rotation * M_PI / 180.0f;
          float rotatedX =
              std::cos(radians) * offsetX - std::sin(radians) * offsetY;
          float rotatedY =
              std::sin(radians) * offsetX + std::cos(radians) * offsetY;

          animation.destRect.x =
              centerX + rotatedX - animation.destRect.w / 2.0f;
          animation.destRect.y =
              centerY + rotatedY - animation.destRect.h / 2.0f;
          animation.angle = Entity.rotation;
          break;
        }
      }

      Entity.velocity.x = Map::lerp(Entity.velocity.x, forceX, 0.01f);
      Entity.velocity.y = Map::lerp(Entity.velocity.y, forceY, 0.01f);
    }
  }
}
