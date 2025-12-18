#include "EnemyNew.h"
#include "Bullet.h"
#include "Loot.h"
#include "Map.h"
#include "Player.h"
#include "Sound.h"
#include "TextureManager.h"
#include "UI.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>

float Enemy::basicDelay[4] = {3000, 4000, 5000, 6000};
std::vector<EnemyWave> Enemy::Waves;
std::vector<Rotation_ID> Enemy::enemyRotations;

static constexpr Vector SpawnPositions[] = {
    {-400, -400}, {960, -400},  {2320, -400}, {-400, 1280},
    {960, 1580},  {2320, 1280}, {-400, 540},  {2320, 540}};

Enemy::Enemy() : Entity() {
  HP = 1.0f;
  BC = 1.0f;
  x = 0.0f;
  y = 0.0f;
  src = {};
  dest = {0, 0, 64, 60};
  TextureID = 1;
  velocity = {0.0f, 0.0f};
  rotation = 0;
  shooting_delay = 0;
  effectTimer = 0;
}

void Enemy::update(GameManager &gm) {
  if (shooting_delay > 0) {
    shooting_delay -= gm.getDelta();
  }

  if (effectTimer > 0) {
    effectTimer -= gm.getDelta();
  }

  updatePosition(gm.getDelta(), gm.getSpeed());

  Vector vel = getVelocity();
  int entityType = getTypeID();

  if (vel.x != 0.0f || vel.y != 0.0f) {
    bool found = false;
    SDL_FRect a = getDestRect();
    float centerX = a.x + a.w / 2.0f;
    float centerY = a.y + a.h / 2.0f;

    if (entityType == 1) {
      a.h *= 2.0f;
      a.w *= 2.0f;
    } else if (entityType == 2) {
      a.w *= 1.1f;
      a.h *= 1.8f;
    } else if (entityType == 3) {
      a.h *= 1.5f;
      a.w *= 1.9f;
    } else if (entityType == 4) {
      a.h *= 1.9f;
      a.w *= 2.1f;
    }

    a.x = centerX - a.w / 2.0f;
    a.y = centerY - a.h / 2.0f;

    for (auto &animation : TextureManager::animationsVec) {
      if (animation.AnimationNumber == 8 + entityType &&
          animation.EUID == UID) {
        animation.destRect = a;
        animation.angle = rotation;
        found = true;
        break;
      }
    }

    if (!found) {
      TextureManager::animationsVec.emplace_back(0, 10, 300, rotation,
                                                 8 + entityType, a, UID);
    }
  } else {
    TextureManager::animationsVec.erase(
        std::remove_if(TextureManager::animationsVec.begin(),
                       TextureManager::animationsVec.end(),
                       [this, entityType](const AnimationVector &anim) {
                         return anim.AnimationNumber == 8 + entityType &&
                                anim.EUID == UID;
                       }),
        TextureManager::animationsVec.end());
  }
}

void Enemy::onDeath(GameManager &gm) {
  gm.addScore(getScoreValue());
  gm.addCoins(getCoinDrop());
  Player::enemies_killed++;
  Player::PlayerUpgrades.ExperienceP += getExperienceDrop();
}

int Enemy::Randomizer(int min, int max) {
  std::random_device rd;
  std::uniform_int_distribution<int> distrib(min, max);
  return distrib(rd);
}

static int currentWaveIndex = 0;
static int aclock = 0;

void Enemy::RestartWaves() {
  currentWaveIndex = 0;
  for (size_t t = 0; t < Waves.size(); t++) {
    Waves[t].activated = false;
    Waves[t].delayBetweenWaves = basicDelay[t];
  }
}

void Enemy::SwarmingScenario() {
  if (currentWaveIndex >= static_cast<int>(Waves.size())) {
    if (GameManagerSingleton::instance()
            .getEntityManager()
            .getEntities()
            .size() == 1) {
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
          Entity *enemy = SpawnEnemy(currentWave.EnemyTypes[t]);
          if (enemy) {
            GameManagerSingleton::instance().getEntityManager().add(enemy);
          }
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

Entity *Enemy::SpawnEnemy(int type) {
  Entity *enemy = nullptr;

  switch (type) {
  case 1:
    enemy = new BasicEnemy();
    break;
  case 2:
    enemy = new TorpedoEnemy();
    break;
  case 3:
    enemy = new DreadnoughtEnemy();
    break;
  case 4:
    enemy = new BattlecruiserEnemy();
    break;
  default:
    return nullptr;
  }

  float spawnX = SpawnPositions[Randomizer(0, 7)].x;
  float spawnY = SpawnPositions[Randomizer(0, 7)].y;

  if (spawnX > 0 &&
      spawnX <
          static_cast<float>(GameManagerSingleton::instance().getWindowW()) &&
      spawnY > 0 &&
      spawnY <
          static_cast<float>(GameManagerSingleton::instance().getWindowH())) {
    if (Randomizer(0, 1) == 0)
      spawnX = (Randomizer(0, 1) == 0)
                   ? -200
                   : GameManagerSingleton::instance().getWindowW() + 200;
    else
      spawnY = (Randomizer(0, 1) == 0)
                   ? -200
                   : GameManagerSingleton::instance().getWindowH() + 200;
  }

  enemy->setPosition(spawnX, spawnY);

  uint32_t uid =
      GameManagerSingleton::instance().getEntityManager().generateID(type);
  enemy->setUID(uid);

  if (type == 1) {
    BasicEnemy *basic = static_cast<BasicEnemy *>(enemy);
    float orbitDir = static_cast<float>(Randomizer(-2, 2));
    basic->setOrbitDirection(orbitDir);
    enemyRotations.emplace_back(Rotation_ID{static_cast<int>(orbitDir), uid});
  } else if (type == 4) {
    BattlecruiserEnemy *battlecruiser =
        static_cast<BattlecruiserEnemy *>(enemy);
    float orbitDir = static_cast<float>(Randomizer(-1, 1));
    battlecruiser->setOrbitDirection(orbitDir);
    enemyRotations.emplace_back(Rotation_ID{static_cast<int>(orbitDir), uid});
  }

  return enemy;
}

BasicEnemy::BasicEnemy() : Enemy() {
  HP = 1.0f;
  dest.w = 64;
  dest.h = 60;
  TextureID = 1;
  type = 1;
  orbitDirection = 0.0f;
}

void BasicEnemy::updateAI(GameManager &gm, Entity &player, double deltaTime) {
  float forceX = 0.0f, forceY = 0.0f;
  float radius = 400.0f;

  float dx = player.getDestRect().x - x;
  float dy = player.getDestRect().y - y;
  float distance = std::sqrt(dx * dx + dy * dy);

  if (distance > radius) {
    forceX += (dx / distance) * 0.5f;
    forceY += (dy / distance) * 0.5f;
  }

  for (Entity *otherEntity : gm.getEntityManager().getEntities()) {
    if (!otherEntity || UID == otherEntity->getUID())
      continue;

    float diffX = x - otherEntity->getX();
    float diffY = y - otherEntity->getY();
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

  float angle = std::atan2(dy, dx);
  float orbitForceX = std::sin(angle) * 0.5f * orbitDirection;
  float orbitForceY = -std::cos(angle) * 0.5f * orbitDirection;

  forceX += orbitForceX;
  forceY += orbitForceY;

  rotation = (std::atan2(dy, dx) * (180.0f / M_PI)) + 90;

  if (shooting_delay <= 0.0f) {
    Bullet::spawnBulletEnemy({dest.x, dest.y}, {dest.w, dest.h}, rotation, UID);
    shooting_delay = 200;
  }

  velocity.x = Map::lerp(velocity.x, forceX, 0.1f);
  velocity.y = Map::lerp(velocity.y, forceY, 0.1f);
}

TorpedoEnemy::TorpedoEnemy() : Enemy() {
  HP = 1.0f;
  dest.w = 84;
  dest.h = 58;
  TextureID = 2;
  type = 2;
}

void TorpedoEnemy::updateAI(GameManager &gm, Entity &player, double deltaTime) {
  float forceX = 0.0f, forceY = 0.0f;

  float dx = player.getDestRect().x - x;
  float dy = player.getDestRect().y - y;
  float distance = std::sqrt(dx * dx + dy * dy);

  if (distance > 20) {
    forceX += (dx / distance) * 0.7f;
    forceY += (dy / distance) * 0.7f;
  }

  rotation = std::atan2(dy, dx) * (180.0f / M_PI) + 90;

  if (SDL_HasRectIntersectionFloat(&player.getDestRectRef(), &dest)) {
    SDL_FRect result;
    SDL_GetRectIntersectionFloat(&player.getDestRectRef(), &dest, &result);

    result.w = std::max(dest.w, result.w);
    result.h = std::max(dest.h, result.h);

    TextureManager::animationsVec.emplace_back(0, 20, 200, 0, 1, result, UID);
    HP = -0.9999f;
    player.takeDamage(0.2f * GameManagerSingleton::instance().getDifficulty());
    player.setEffectTimer(40);
    Player::hitPlayerLevel = true;
    Player::hitPlayerWave = true;

    float angle = std::atan2(dest.y - result.y, dest.x - result.x);
    Vector playerVel = player.getVelocity();
    playerVel.x =
        Map::lerp(playerVel.x, playerVel.x - std::cos(angle) * 0.4, 0.1f);
    playerVel.y =
        Map::lerp(playerVel.y, playerVel.y - std::sin(angle) * 0.4, 0.1f);
    player.setVelocity(playerVel);
    Sound::PlaySound(2);
  }

  for (Entity *otherEntity : gm.getEntityManager().getEntities()) {
    if (!otherEntity || UID == otherEntity->getUID())
      continue;

    float diffX = x - otherEntity->getX();
    float diffY = y - otherEntity->getY();
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

  velocity.x = Map::lerp(velocity.x, forceX, 0.2f);
  velocity.y = Map::lerp(velocity.y, forceY, 0.2f);
}

void TorpedoEnemy::onDeath(GameManager &gm) {
  gm.addScore(getScoreValue());
  Player::enemies_killed++;
}

DreadnoughtEnemy::DreadnoughtEnemy() : Enemy() {
  HP = 3.0f;
  dest.w = 68 * 2;
  dest.h = 98 * 2;
  TextureID = 3;
  type = 3;
  rotation = 180;
  shooting_delay = 1200;
}

void DreadnoughtEnemy::updateBeamWeapon(GameManager &gm) {
  for (size_t t = 0; t < TextureManager::animationsVec.size(); t++) {
    if (TextureManager::animationsVec[t].EUID == UID) {
      if (TextureManager::animationsVec[t].AnimationNumber == 4) {
        TextureManager::animationsVec[t].angle = rotation - 180;
        float angleRad = (rotation - 90.0f) * (M_PI / 180.0f);
        float beamOffset = 100.0f;

        float cosA = std::cos(angleRad);
        float sinA = std::sin(angleRad);
        float HalfWidth = TextureManager::animationsVec[t].destRect.w / 2;
        float Height = TextureManager::animationsVec[t].destRect.h;

        float originX = x + dest.w / 2 + cosA * beamOffset;
        float originY = y + dest.h / 2 + sinA * beamOffset;

        angleRad = angleRad + M_PI / 7.0f;
        cosA = std::cos(angleRad - 90);
        sinA = std::sin(angleRad - 90);

        TextureManager::animationsVec[t].destRect.x = originX - HalfWidth;
        TextureManager::animationsVec[t].destRect.y = originY;

        SDL_FPoint localCorners[6] = {{-HalfWidth / 2, 0.0f},
                                      {HalfWidth / 2, 0.0f},
                                      {HalfWidth / 2, Height},
                                      {-HalfWidth / 2, Height},
                                      {0.0f, 0.0f},
                                      {0.0f, Height}};

        SDL_FPoint Beamcorners[6];
        for (int i = 0; i < 6; ++i) {
          float lx = localCorners[i].x;
          float ly = localCorners[i].y;
          float rx = lx * cosA - ly * sinA;
          float ry = lx * sinA + ly * cosA;
          Beamcorners[i].x = originX + rx;
          Beamcorners[i].y = originY + ry;
        }

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
        for (Entity *entity : gm.getEntityManager().getEntities()) {
          if (!entity)
            continue;
          for (int i = 0; i < 3; i++) {
            SDL_FRect entityDest = entity->getDestRect();
            if (entity->getTypeID() != 3 &&
                SDL_GetRectAndLineIntersectionFloat(
                    &entityDest, &LinesForChecking[i][0].x,
                    &LinesForChecking[i][0].y, &LinesForChecking[i][1].x,
                    &LinesForChecking[i][1].y)) {
              TextureManager::animationsVec[t].destRect.h =
                  Map::lerp(TextureManager::animationsVec[t].destRect.h,
                            LinesForChecking[i][1].y -
                                TextureManager::animationsVec[t].destRect.y,
                            0.05f);

              float currentHP = entity->getHP();
              entity->setHP(Map::lerp(
                  currentHP,
                  currentHP -
                      (0.15f *
                       GameManagerSingleton::instance().getDifficulty()),
                  0.02f));
              entity->setEffectTimer(15);
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
          TextureManager::animationsVec[t].destRect.h =
              Map::lerp(TextureManager::animationsVec[t].destRect.h, 748, 0.1f);
        }
      }

      if (TextureManager::animationsVec[t].AnimationNumber == 5 &&
          TextureManager::animationsVec[t].EUID == UID) {
        TextureManager::animationsVec[t].destRect.x = dest.x - dest.w / 2.3;
        TextureManager::animationsVec[t].destRect.y = dest.y - dest.h / 5.7;
        TextureManager::animationsVec[t].angle = rotation;
      }
    }
  }
}

void DreadnoughtEnemy::updateAI(GameManager &gm, Entity &player,
                                double deltaTime) {
  float forceX = 0.0f, forceY = 0.0f;
  float dy = 0;
  float dx = player.getDestRect().x - x;

  y = Map::lerp(y, 100.0f, 0.02f);
  if (y <= 60 || y >= 140) {
    dy = 100.0f - y;
  }

  float distance = std::sqrt(dx * dx + dy * dy);

  forceX += (dx / distance) * 0.7f;
  forceY += (dy / distance) * 0.7f;

  dx = player.getX() - x;
  dy = player.getY() - y;
  float current = rotation;
  float target =
      std::fmod(std::atan2(dy, dx) * (180.0f / M_PI) + 450.0f, 360.0f);
  float delta = std::fmod((target - current + 540.0f), 360.0f) - 180.0f;
  rotation += delta * 0.02f;

  if (shooting_delay <= 450.0f && shooting_delay >= 350.0f) {
    SDL_FRect shipanim = {dest.x - dest.w / 2, dest.y - dest.h / 2, 256, 256};
    TextureManager::animationsVec.emplace_back(0, 20, 800, 180, 5, shipanim,
                                               UID);
    Sound::PlaySound(7);
  }

  if (shooting_delay <= 0.0f) {
    SDL_FRect a = {x + dest.w / 2 - 128, y + dest.h, 256, 1};
    TextureManager::animationsVec.emplace_back(0, 1, 450, rotation - 180, 4, a,
                                               UID);
    Sound::PlaySound(8);
    shooting_delay = 1200;
  }

  updateBeamWeapon(gm);

  for (Entity *otherEntity : gm.getEntityManager().getEntities()) {
    if (!otherEntity || UID == otherEntity->getUID())
      continue;

    float diffX = x - otherEntity->getX();
    float diffY = y - otherEntity->getY();
    float dist = std::sqrt(diffX * diffX + diffY * diffY);

    if (dist < 500 && dist > 230) {
      float repelForce = 0.4f / dist;
      forceX += (diffX / dist) * repelForce * 4.0f;
    } else if (dist <= 230) {
      forceX += -1.5f * static_cast<float>(Randomizer(-2, 2));
    }
  }

  velocity.x = Map::lerp(velocity.x, forceX, 0.008f);
  velocity.y = Map::lerp(velocity.y, forceY, 0.04f);

  dest.y = y;
}

BattlecruiserEnemy::BattlecruiserEnemy() : Enemy() {
  HP = 6.0f;
  dest.w = 62 * 2;
  dest.h = 85 * 2;
  TextureID = 4;
  type = 4;
  shooting_delay = 700;
  orbitDirection = 0.0f;
}

void BattlecruiserEnemy::updateAI(GameManager &gm, Entity &player,
                                  double deltaTime) {
  float forceX = 0.0f, forceY = 0.0f;
  float radius = 400.0f;

  float dx = player.getDestRect().x - x;
  float dy = player.getDestRect().y - y;
  float distance = std::sqrt(dx * dx + dy * dy);

  if (distance > radius + 100) {
    forceX += (dx / distance) * 0.5f;
    forceY += (dy / distance) * 0.5f;
  }

  for (Entity *otherEntity : gm.getEntityManager().getEntities()) {
    if (!otherEntity || UID == otherEntity->getUID())
      continue;

    float diffX = x - otherEntity->getX();
    float diffY = y - otherEntity->getY();
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

  float angle = std::atan2(dy, dx);
  float orbitForceX = std::sin(angle) * 0.5f * orbitDirection;
  float orbitForceY = -std::cos(angle) * 0.5f * orbitDirection;

  forceX += orbitForceX;
  forceY += orbitForceY;

  rotation = (std::atan2(dy, dx) * (180.0f / M_PI)) + 90;

  if (shooting_delay <= 0.0f) {
    SDL_FRect shipanim = {dest.x, dest.y, 256, 256};
    TextureManager::animationsVec.emplace_back(0, 20, 400, rotation, 7,
                                               shipanim, UID);

    float baseAngle = (rotation - 95) * (M_PI / 180.0f);
    float spread = M_PI / 8.0f;
    for (int i = 0; i < 12; i++) {
      float angle = baseAngle - spread / 2 + spread * (i / 9.0f);
      Bullet::SpawnBattlecruiserBullet(dest.x + dest.w / 2, dest.y + dest.h / 2,
                                       angle, angle, 0, 0.1f, UID);
    }
    shooting_delay = 450;
    Sound::PlaySound(15);
  }

  for (auto &animation : TextureManager::animationsVec) {
    if (animation.AnimationNumber == 7 && animation.EUID == UID) {
      float centerX = dest.x + dest.w / 2.0f;
      float centerY = dest.y + dest.h / 2.0f;
      float offsetX = 0.0f;
      float offsetY = dest.h / 11.0f;

      float radians = rotation * M_PI / 180.0f;
      float rotatedX =
          std::cos(radians) * offsetX - std::sin(radians) * offsetY;
      float rotatedY =
          std::sin(radians) * offsetX + std::cos(radians) * offsetY;

      animation.destRect.x = centerX + rotatedX - animation.destRect.w / 2.0f;
      animation.destRect.y = centerY + rotatedY - animation.destRect.h / 2.0f;
      animation.angle = rotation;
      break;
    }
  }

  velocity.x = Map::lerp(velocity.x, forceX, 0.01f);
  velocity.y = Map::lerp(velocity.y, forceY, 0.01f);
}
