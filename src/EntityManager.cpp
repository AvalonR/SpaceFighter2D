#include "EnemyNew.h"
#include "Entity.h"
#include "Loot.h"
#include "Player.h"
#include "Setup.h"
#include "Sound.h"
#include "TextureManager.h"
#include "UI.h"
#include <algorithm>

EntityManager::~EntityManager() { clear(); }

void EntityManager::clear() {
  for (auto *entity : entities) {
    delete entity;
  }
  entities.clear();
  usedIDs.clear();
}

void EntityManager::removeDeadEntities(GameManager &gm) {
  auto it = entities.begin();
  while (it != entities.end()) {
    Entity *entity = *it;

    if (entity->isDead()) {
      if (entity->getTypeID() == 0) {
        Enemy::RestartWaves();
        gm.setRestart(true);
        UI::PushState(UIState::RESTART_CONFIRMATION);
        ++it;
        continue;
      }

      SDL_FRect dest = entity->getDestRect();
      Vector vel = entity->getVelocity();
      SDL_FRect explosionRect = {dest.x + vel.x * gm.getSpeed(),
                                 dest.y + vel.y * gm.getSpeed(), dest.w,
                                 dest.h};
      SDL_FRect coinRect = {dest.x + vel.x * gm.getSpeed(),
                            dest.y + vel.y * gm.getSpeed(), 8, 8};

      int type = entity->getTypeID();

      if (type == 1) {
        explosionRect.h *= 2;
        explosionRect.w *= 2;

        for (auto &animation : TextureManager::animationsVec) {
          if (animation.AnimationNumber == 9 &&
              animation.EUID == entity->getUID()) {
            animation.expiration = -1;
          }
        }

        TextureManager::animationsVec.emplace_back(
            0, 20, 300, entity->getRotation(), 2, explosionRect,
            entity->getUID());
        Sound::PlaySound(9);
        Loot::spawnCoins(coinRect, 10);
      }

      else if (type == 2 && entity->getHP() != -0.9999f) {
        explosionRect.w *= 1.2;
        explosionRect.h *= 1.7;

        for (auto &animation : TextureManager::animationsVec) {
          if (animation.AnimationNumber == 10 &&
              animation.EUID == entity->getUID()) {
            animation.expiration = -1;
          }
        }

        TextureManager::animationsVec.emplace_back(
            0, 20, 300, entity->getRotation(), 0, explosionRect,
            entity->getUID());
        Sound::PlaySound(9);
        Loot::spawnCoins(coinRect, 15);
      }

      else if (type == 2 && entity->getHP() == -0.9999f) {
        explosionRect.w *= 1.2;
        explosionRect.h *= 1.7;

        for (auto &animation : TextureManager::animationsVec) {
          if (animation.AnimationNumber == 10 &&
              animation.EUID == entity->getUID()) {
            animation.expiration = -1;
          }
        }

        TextureManager::animationsVec.emplace_back(
            0, 20, 300, entity->getRotation(), 0, explosionRect,
            entity->getUID());
      }

      else if (type == 3) {
        explosionRect.x = entity->getX();
        explosionRect.y = entity->getY();
        explosionRect.w = 256;
        explosionRect.h = 256;

        for (auto &animation : TextureManager::animationsVec) {
          if ((animation.AnimationNumber == 4 &&
               animation.EUID == entity->getUID()) ||
              (animation.AnimationNumber == 5 &&
               animation.EUID == entity->getUID()) ||
              (animation.AnimationNumber == 11 &&
               animation.EUID == entity->getUID())) {
            animation.expiration = -1;
          }
        }

        TextureManager::animationsVec.emplace_back(
            0, 20, 300, entity->getRotation(), 3, explosionRect,
            entity->getUID());
        Sound::PlaySound(9);
        Loot::spawnCoins(coinRect, 100);
      }

      else if (type == 4) {
        explosionRect.x = entity->getX();
        explosionRect.y = entity->getY();
        explosionRect.w = 256;
        explosionRect.h = 256;

        for (auto &animation : TextureManager::animationsVec) {
          if ((animation.AnimationNumber == 7 &&
               animation.EUID == entity->getUID()) ||
              (animation.AnimationNumber == 12 &&
               animation.EUID == entity->getUID())) {
            animation.expiration = -1;
          }
        }

        TextureManager::animationsVec.emplace_back(
            0, 20, 300, entity->getRotation(), 6, explosionRect,
            entity->getUID());
        Sound::PlaySound(9);
        Loot::spawnCoins(coinRect, 150);
      }

      entity->onDeath(gm);

      delete entity;
      it = entities.erase(it);
    } else {
      ++it;
    }
  }
}

std::vector<EntityStats> EntityManager::getLegacyEntities() {
  std::vector<EntityStats> legacyEntities;
  for (Entity *entity : entities) {
    EntityStats stats;
    stats.HP = entity->getHP();
    stats.BC = entity->getBC();
    stats.type = entity->getTypeID();
    stats.x = entity->getX();
    stats.y = entity->getY();
    stats.TextureID = entity->getTextureID();
    stats.src = entity->getSrcRect();
    stats.dest = entity->getDestRect();
    stats.rotation = entity->getRotation();
    stats.velocity = entity->getVelocity();
    stats.shooting_delay = entity->getShootingDelay();
    stats.effectTimer = entity->getEffectTimer();
    stats.UID = entity->getUID();
    legacyEntities.push_back(stats);
  }
  return legacyEntities;
}
