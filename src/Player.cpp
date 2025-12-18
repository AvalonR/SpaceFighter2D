#include "Player.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <ostream>

#include "Bullet.h"
#include "Setup.h"
#include "Sound.h"
#include "TextureManager.h"
#include "UI.h"

bool Player::LeftMouse = false;
bool Player::RightMouse = false;
std::unordered_map<SDL_Keycode, bool> Player::keyState;

Player::Player() : Entity() {
  HP = 1.0f;
  BC = 1.0f;
  x = y = 800.0;
  src = {};
  dest = {800, 500, 84, 84};
  TextureID = 0;
  velocity.x = velocity.y = 0;
  rotation = 0;
  shooting_delay = 0;
  type = 0;
}

void Player::update(GameManager &gm) {
  updatePosition(gm.getDelta(), gm.getSpeed() * PlayerUpgrades.MovementSpeed);

  SDL_FRect dest = getDestRect();
  float clampedX = std::clamp(x, 0.0f, gm.getWindowW() - dest.w);
  float clampedY = std::clamp(y, 0.0f, gm.getWindowH() - dest.h);
  setPosition(clampedX, clampedY);

  // Update effect timer
  if (effectTimer > 0) {
    effectTimer -= gm.getDelta();
  }

  // Handle engine trail animation
  Vector vel = getVelocity();
  if (vel.x != 0.0f || vel.y != 0.0f) {
    bool found = false;
    SDL_FRect a = getDestRect();
    float centerX = a.x + a.w / 2.0f;
    float centerY = a.y + a.h / 2.0f;

    a.h *= 1.6f;
    a.w *= 1.6f;
    a.x = centerX - a.w / 2.0f;
    a.y = centerY - a.h / 2.0f;

    for (auto &animation : TextureManager::animationsVec) {
      if (animation.AnimationNumber == 8 && animation.EUID == UID) {
        animation.destRect = a;
        animation.angle = rotation;
        found = true;
        break;
      }
    }

    if (!found) {
      TextureManager::animationsVec.emplace_back(0, 10, 300, rotation, 8, a,
                                                 UID);
    }
  } else {
    TextureManager::animationsVec.erase(
        std::remove_if(TextureManager::animationsVec.begin(),
                       TextureManager::animationsVec.end(),
                       [this](const AnimationVector &anim) {
                         return anim.AnimationNumber == 8 && anim.EUID == UID;
                       }),
        TextureManager::animationsVec.end());
  }
}

void Player::onDeath(GameManager &gm) {
  // Handle player death
}
float PlayerReloadingCooldown = 0;
bool isReloading = false;
int Player::enemies_killed = 0;
int Player::bosses_killed = 0;
int Player::shots_landed = 0;
int Player::coin_pickup_played = 0;
bool Player::finished_level = false;
bool Player::upgradedPlayer = false;
bool Player::hitPlayerWave = false;
bool Player::hitPlayerLevel = false;
PlayerUpgradeStats Player::PlayerUpgrades;
Achieve Player::Achievements[15];
int completedachivements = 0;
void Player::AchievementLogging(GameManager &gm) {
  Achievements[0].title = "First Blood";
  if (enemies_killed == 1 && !Achievements[0].achieved) {
    Achievements[0].achieved = true;
    Achievements[0].description =
        "Took down your very first enemy. The war begins.";
    completedachivements++;
  }
  Achievements[1].title = "Coming In Hot";
  if (enemies_killed == 10 && !Achievements[1].achieved) {
    Achievements[1].achieved = true;
    Achievements[1].description =
        "You're not just lucky — you're getting good. 10 foes down.";
    completedachivements++;
  }
  Achievements[2].title = "Wrecking Machine";
  if (enemies_killed == 100 && !Achievements[2].achieved) {
    Achievements[2].achieved = true;
    Achievements[2].description =
        "Left a hundred enemies in ruins. And you're just warming up.";
    completedachivements++;
  }
  Achievements[3].title = "Boss Slayer";
  if (bosses_killed == 1 && !Achievements[3].achieved) {
    Achievements[3].achieved = true;
    Achievements[3].description =
        "You've slain your first boss. That roar? It was your own.";
    completedachivements++;
  }
  Achievements[4].title = "Boss Massacre";
  if (bosses_killed == 10 && !Achievements[4].achieved) {
    Achievements[4].achieved = true;
    Achievements[4].description =
        "Ten bosses fell before your might. Are you even mortal?";
    completedachivements++;
  }
  Achievements[5].title = "Treasure Hunter";
  if (gm.getTargetCoins() >= 100 && !Achievements[5].achieved) {
    Achievements[5].achieved = true;
    Achievements[5].description =
        "Collected 100 coins. Wealth favors the brave.";
    completedachivements++;
  }
  Achievements[6].title = "Sound of Gold";
  if (coin_pickup_played >= 100 && !Achievements[6].achieved) {
    Achievements[6].achieved = true;
    Achievements[6].description = "You've heard it enough to dream in coins. "
                                  "100 pickups later, still satisfying..";
    completedachivements++;
  }
  Achievements[7].title = "Survivor's Instinct";
  Entity *player = gm.getEntityManager().getPlayer();
  if (finished_level && player && player->getHP() < 0.3f &&
      !Achievements[7].achieved) {
    Achievements[7].achieved = true;
    Achievements[7].description =
        "Barely made it out alive. But alive is alive.";
    completedachivements++;
  }
  Achievements[8].title = "Wanderer of the Stars";
  if (gm.getCurrentLevel() >= 3 && !Achievements[8].achieved) {
    Achievements[8].achieved = true;
    Achievements[8].description =
        "No boundary is too distant for a curious soul. Visited many a level";
    completedachivements++;
  }
  Achievements[9].title = "Dodger Supreme";
  if (!hitPlayerWave && !Achievements[9].achieved) {
    Achievements[9].achieved = true;
    Achievements[9].description =
        "Not a scratch. You dance between bullets like wind.";
    completedachivements++;
  }
  Achievements[10].title = "Armed and Ready";
  if (upgradedPlayer && !Achievements[10].achieved) {
    Achievements[10].achieved = true;
    Achievements[10].description =
        "You’ve reached most serious firepower. Let them come.";
    completedachivements++;
  }
  Achievements[11].title = "Laser Surgeon";
  if (shots_landed > 100 && !Achievements[11].achieved) {
    Achievements[11].achieved = true;
    Achievements[11].description = "Your aim cuts cleaner than steel.";
    completedachivements++;
  }
  Achievements[12].title = "Untouchable";
  if (!hitPlayerLevel && finished_level && !Achievements[12].achieved) {
    Achievements[12].achieved = true;
    Achievements[12].description = "They tried. They missed. Every single one.";
    completedachivements++;
  }
  Achievements[13].title = "Wavebreaker";
  if (finished_level && !Achievements[13].achieved) {
    Achievements[13].achieved = true;
    Achievements[13].description = "No wave too wild. You crushed them all.";
    completedachivements++;
  }
  Achievements[14].title = "Completionist";
  if (completedachivements == 14) {
    Achievements[14].achieved = true;
    Achievements[14].description =
        "Every feat, every challenge — complete. You're legendary.";
  }
}
float healingcooldown = 144.0f;
bool Player::levelUpSoundPlayed = false;
void Player::playerInput(GameManager &gm) {
  float delta = gm.getDelta();
  Entity *player = gm.getEntityManager().getPlayer();
  if (!player)
    return;

  if (PlayerUpgrades.ExperienceP >= 1.0f && UI::ContainsSwarmPlaying()) {
    if (!levelUpSoundPlayed) {
      levelUpSoundPlayed = true;
      Sound::PlaySound(10);
    }
    if (UI::GetCurrentState() != UIState::LEVEL_UP_MENU) {
      UI::PushState(UIState::LEVEL_UP_MENU);
    }
    gm.set_is_Paused(true);
    UI::LevelUpChosen = false;
    UI::LevelUp(gm);
  }
  if (!gm.get_is_Paused()) {
    healingcooldown -= delta;
    PlayerReloadingCooldown -= delta;
    float shootDelay = player->getShootingDelay() - delta;
    player->setShootingDelay(shootDelay);
  }
  if (player->getHP() < Player::PlayerUpgrades.TotalHP && healingcooldown < 0) {
    if (!gm.get_is_Paused()) {
      player->setHP(
          std::min(player->getHP() * Player::PlayerUpgrades.PassiveHealing,
                   Player::PlayerUpgrades.TotalHP));
      healingcooldown = 144.0f;
    }
  }
  if (player->getBC() >= PlayerUpgrades.TotalBulletCap) {
    isReloading = false;
  }
  if (isReloading == true && PlayerReloadingCooldown <= 0) {
    if (!gm.get_is_Paused()) {
      Sound::PlaySound(14);
      player->setBC(player->getBC() + 0.1f);
      PlayerReloadingCooldown =
          static_cast<int>(72 * PlayerUpgrades.bulletCooldownMultiplier);
    }
  }
  SDL_GetMouseState(&gm.getMouseCoordin().x, &gm.getMouseCoordin().y);
  // Calculate deltas using double precision
  SDL_FRect playerDest = player->getDestRect();
  double deltaX = static_cast<double>(gm.getMouseCoordin().x) -
                  static_cast<double>(playerDest.x + playerDest.w / 2);
  double deltaY = static_cast<double>(gm.getMouseCoordin().y) -
                  static_cast<double>(playerDest.y + playerDest.h / 2);

  double angleDegrees = (std::atan2(deltaY, deltaX) * (180.0f / M_PI)) + 90.0;

  while (SDL_PollEvent(&gm.getEvent())) {
    if (gm.getEvent().type == SDL_EVENT_QUIT) {
      gm.setloopRunning(false);
      return;
    }

    if (gm.getEvent().type == SDL_EVENT_KEY_DOWN) {
      keyState[gm.getEvent().key.key] = true;

      if (gm.getEvent().key.key == SDLK_ESCAPE) {
        if (UI::GetCurrentState() != UIState::SETTINGS_MENU &&
            UI::GetCurrentState() != UIState::ACHIEVEMENTS &&
            UI::GetCurrentState() != UIState::SCORE_BOARD) {
          UI::PushState(UIState::SETTINGS_MENU);
        } else if (UI::GetCurrentState() == UIState::SETTINGS_MENU) {
          UI::PopState();
        } else if (UI::GetCurrentState() == UIState::DIFFICULTY_SELECTION ||
                   UI::GetCurrentState() == UIState::STORE_MENU ||
                   UI::GetCurrentState() == UIState::LEVEL_UP_MENU ||
                   UI::GetCurrentState() == UIState::ACHIEVEMENTS ||
                   UI::GetCurrentState() == UIState::SCORE_BOARD) {
          UI::PopState();
        }
      }
    }
    if (gm.getEvent().type == SDL_EVENT_MOUSE_WHEEL) {
      UI::Scrolling -= (gm.getEvent().wheel.y * 10);
    }
    if (gm.getEvent().type == SDL_EVENT_KEY_UP) {
      for (int i = 0; i < 7; i++) {
        UI::ControlsPressed[i] = false;
      }
      keyState[gm.getEvent().key.key] = false;
    }
    player->setVelocity({0.0f, 0.0f});
    if (keyState[SDLK_E]) {
      UI::ControlsPressed[2] = true;
      if (gm.get_is_Paused() != true) {
        if (player->getShootingDelay() <= 0 && player->getBC() >= 0) {
          isReloading = false;
          player->setShootingDelay(100);
          player->setBC(player->getBC() - 0.1f);
          Bullet::spawnBulletPlayer();
        }
      }
    }
    if (keyState[SDLK_Q]) {
      UI::ControlsPressed[0] = true;
      if (gm.get_is_Paused() != true) {
        if (player->getBC() < PlayerUpgrades.TotalBulletCap && !isReloading) {
          isReloading = true;
        }
      }
    }
    if (keyState[SDLK_W]) {
      UI::ControlsPressed[1] = true;
    }
    if (keyState[SDLK_S]) {
      UI::ControlsPressed[4] = true;
    }
    if (keyState[SDLK_A]) {
      UI::ControlsPressed[3] = true;
    }
    if (keyState[SDLK_D]) {
      UI::ControlsPressed[5] = true;
    }
    if (gm.getEvent().type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
      if (gm.getEvent().button.button == SDL_BUTTON_LEFT) {
        LeftMouse = true;
      } else if (gm.getEvent().button.button == SDL_BUTTON_RIGHT) {
        RightMouse = true;
      }
    }
    if (gm.getEvent().type == SDL_EVENT_MOUSE_BUTTON_UP) {
      if (gm.getEvent().button.button == SDL_BUTTON_LEFT) {
        LeftMouse = false;
      } else if (gm.getEvent().button.button == SDL_BUTTON_RIGHT) {
        RightMouse = false;
      }
    }
    if (gm.get_is_Paused())
      return;
    player->setRotation(angleDegrees);

    // Reset velocity each frame, then apply input
    Vector vel = {0.0f, 0.0f};
    if (keyState[SDLK_W]) {
      vel.y -= 1.0f;
      UI::ControlsPressed[1] = true;
    } // Move up
    if (keyState[SDLK_S]) {
      vel.y += 1.0f;
      UI::ControlsPressed[4] = true;
    } // Move down
    if (keyState[SDLK_A]) {
      vel.x -= 1.0f;
      UI::ControlsPressed[3] = true;
    } // Move left
    if (keyState[SDLK_D]) {
      vel.x += 1.0f;
      UI::ControlsPressed[5] = true;
    } // Move right
    player->setVelocity(vel);
  }
}
