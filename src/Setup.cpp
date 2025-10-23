#include "Setup.h"

#include <cstdint>
#include <iostream>
#include <ostream>

#include "Bullet.h"
#include "Enemy.h"
#include "Loot.h"
#include "Map.h"
#include "Player.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "Sound.h"
#include "TextManager.h"
#include "TextureManager.h"
#include "UI.h"

void GameManager::initializationSet() {
  coinSystem.reset();
  scoreSystem.reset();
  levelManager.reset();
  delta = 0;
  is_Paused = true;
  Restart = false;
  Max_FPS = 144.0f;
  loopRunning = true;
  mouseCoordin = {};
  speed = 6.;
  Vsync = false;
  WindowWidth = 1920;
  WindowHeight = 1080;
  window = SDL_CreateWindow("Game2D", WindowWidth, WindowHeight,
                            SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL |
                                SDL_WINDOW_FULLSCREEN);
  if (window == nullptr) {
    std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
    loopRunning = false;
    return;
  }
  std::string assetspath = Setup::basePath + "/assets/icon.png";
  SDL_Surface *icon = IMG_Load(assetspath.c_str());
  if (icon)
    SDL_SetWindowIcon(window, icon);
  SDL_DestroySurface(icon);
  renderer = SDL_CreateRenderer(window, "opengl");
  SDL_SetRenderVSync(renderer, Vsync);
}

Setup Setup;

const std::string Setup::basePath = []() {
  const char *cwd = SDL_GetBasePath();
  std::string basePath = cwd;
  basePath.erase(basePath.length() - 6);
  std::replace(basePath.begin(), basePath.end(), '\\', '/');
  std::cout << "Base path: " << basePath << std::endl;
  return basePath;
}();

std::vector<int> Setup::ScoreHistory;

void EntityManager::removeDeadEntities(GameManager &gm) {
  entities.erase(
      std::ranges::remove_if(
          entities,
          [&gm](EntityStats &Entity) {
            // Player death
            if (Entity.HP <= 0 && Entity.type == 0) {
              Enemy::RestartWaves();
              gm.setRestart(true);
              UI::PushState(UIState::RESTART_CONFIRMATION);
              return false; // Don't remove player yet, restart handles it
            }

            // Enemy death
            if (Entity.HP <= 0 && Entity.type != 0) {
              SDL_FRect explosionRect = {
                  Entity.dest.x + Entity.velocity.x * gm.getSpeed(),
                  Entity.dest.y + Entity.velocity.y * gm.getSpeed(),
                  Entity.dest.w, Entity.dest.h};
              SDL_FRect coinRect = {
                  Entity.dest.x + Entity.velocity.x * gm.getSpeed(),
                  Entity.dest.y + Entity.velocity.y * gm.getSpeed(), 8, 8};

              // Type 1: Basic Enemy
              if (Entity.type == 1) {
                explosionRect.h *= 2;
                explosionRect.w *= 2;

                // Stop engine animation
                for (auto &animation : TextureManager::animationsVec) {
                  if (animation.AnimationNumber == 9 &&
                      animation.EUID == Entity.UID) {
                    animation.expiration = -1;
                  }
                }

                TextureManager::animationsVec.emplace_back(
                    0, 20, 300, Entity.rotation, 2, explosionRect, Entity.UID);
                gm.addScore(20);
                Sound::PlaySound(9);
                Player::PlayerUpgrades.ExperienceP =
                    std::min(Player::PlayerUpgrades.ExperienceP + 0.15f, 1.0f);
                Player::enemies_killed++;
                Loot::spawnCoins(coinRect, 10);
              }

              // Type 2: Torpedo Enemy
              else if (Entity.type == 2 && Entity.HP != -0.9999f) {
                explosionRect.w *= 1.2;
                explosionRect.h *= 1.7;

                for (auto &animation : TextureManager::animationsVec) {
                  if (animation.AnimationNumber == 10 &&
                      animation.EUID == Entity.UID) {
                    animation.expiration = -1;
                  }
                }

                TextureManager::animationsVec.emplace_back(
                    0, 20, 300, Entity.rotation, 0, explosionRect, Entity.UID);
                gm.addScore(25);
                Sound::PlaySound(9);
                Player::PlayerUpgrades.ExperienceP =
                    std::min(Player::PlayerUpgrades.ExperienceP + 0.1f, 1.0f);
                Player::enemies_killed++;
                Loot::spawnCoins(coinRect, 15);
              }

              // Type 2: Torpedo Enemy (special death - no rewards)
              else if (Entity.type == 2 && Entity.HP == -0.9999f) {
                explosionRect.w *= 1.2;
                explosionRect.h *= 1.7;

                for (auto &animation : TextureManager::animationsVec) {
                  if (animation.AnimationNumber == 10 &&
                      animation.EUID == Entity.UID) {
                    animation.expiration = -1;
                  }
                }

                TextureManager::animationsVec.emplace_back(
                    0, 20, 300, Entity.rotation, 0, explosionRect, Entity.UID);
              }

              // Type 3: Dreadnought
              else if (Entity.type == 3) {
                explosionRect.x = Entity.x;
                explosionRect.y = Entity.y;
                explosionRect.w = 256;
                explosionRect.h = 256;

                for (auto &animation : TextureManager::animationsVec) {
                  if ((animation.AnimationNumber == 4 &&
                       animation.EUID == Entity.UID) ||
                      (animation.AnimationNumber == 5 &&
                       animation.EUID == Entity.UID) ||
                      (animation.AnimationNumber == 11 &&
                       animation.EUID == Entity.UID)) {
                    animation.expiration = -1;
                  }
                }

                TextureManager::animationsVec.emplace_back(
                    0, 20, 300, Entity.rotation, 3, explosionRect, Entity.UID);
                Sound::PlaySound(9);
                Player::PlayerUpgrades.ExperienceP =
                    std::min(Player::PlayerUpgrades.ExperienceP + 0.3f, 1.0f);
                gm.addScore(250);
                Player::enemies_killed++;
                Player::bosses_killed++;
                Loot::spawnCoins(coinRect, 100);
              }

              // Type 4: Battlecruiser
              else if (Entity.type == 4) {
                explosionRect.x = Entity.x;
                explosionRect.y = Entity.y;
                explosionRect.w = 256;
                explosionRect.h = 256;

                for (auto &animation : TextureManager::animationsVec) {
                  if ((animation.AnimationNumber == 7 &&
                       animation.EUID == Entity.UID) ||
                      (animation.AnimationNumber == 12 &&
                       animation.EUID == Entity.UID)) {
                    animation.expiration = -1;
                  }
                }

                TextureManager::animationsVec.emplace_back(
                    0, 20, 300, Entity.rotation, 6, explosionRect, Entity.UID);
                Sound::PlaySound(9);
                Player::PlayerUpgrades.ExperienceP =
                    std::min(Player::PlayerUpgrades.ExperienceP + 0.3f, 1.0f);
                gm.addScore(350);
                Player::enemies_killed++;
                Player::bosses_killed++;
                Loot::spawnCoins(coinRect, 150);
              }

              return true; // Remove this entity
            }

            return false; // Keep entity alive
          })
          .begin(),
      entities.end());
}
void Setup::initialization(GameManager &gm) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
  }
  if (!SDL_Init(SDL_INIT_EVENTS)) {
    std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
  }
  if (!SDL_Init(SDL_INIT_AUDIO)) {
    std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
  }
  if (!MIX_Init()) {
    std::cout << "SDL_Mixer could not initialize! SDL_mixer Error: "
              << SDL_GetError() << std::endl;
  }
  if (!TTF_Init()) {
    std::cerr << "SDL_ttf could not initialize! TTF_Error: " << SDL_GetError()
              << std::endl;
  }
  GameManagerSingleton::instance().initializationSet();
  UI::Init();
  Sound::Init();
  TextureManager::Init();
  Sound::RandomizeMusic();
  Map::MapGeneration();
  gm.setCurrentLevel(1);
  Map::NewMap(gm.getCurrentLevel());
  Player player;
  EntityStats stats;
  stats.HP = player.HP;
  stats.BC = player.BC;
  stats.type = 0;
  stats.x = player.x;
  stats.y = player.y;
  stats.TextureID = player.TextureID;
  stats.src = player.srcR;
  stats.dest = player.dstR;
  stats.rotation = player.rotation;
  stats.velocity = player.velocity;
  stats.shooting_delay = player.shooting_delay;
  stats.effectTimer = 0;
  stats.UID = gm.generateEntityID(0);
  gm.getEntityManager().add(stats);
  gm.reset();
  Enemy::SpawnEnemy(3);
}

void Setup::gameLoop(GameManager &gm) {
  unsigned int a = SDL_GetTicks();
  unsigned int b = SDL_GetTicks();
  unsigned int fpsTimer = SDL_GetTicks();
  int frames = 0;
  int fps = 0;

  const float REFERENCE_DELTA = 1000.0f / 144.0f; // 6.94ms

  while (gm.getloopRunning()) {
    if (!gm.getVsync()) {
      a = SDL_GetTicks();
      float frameTime = static_cast<float>(a - b);

      const float TARGET_FRAME_TIME = 1000.0f / gm.getMax_FPS();

      if (frameTime >= TARGET_FRAME_TIME) {
        b = a;

        float delta = frameTime / REFERENCE_DELTA;

        delta = std::clamp(delta, 0.1f, 5.0f);

        gm.setDelta(delta);

        if (!UI::Main_Menu(gm)) {
          update(gm);
          render(gm);
        }

        frames++;
        if (SDL_GetTicks() - fpsTimer >= 1000) {
          fps = frames;
          frames = 0;
          fpsTimer += 1000;
          gm.setFPS(fps);
        }
      }
    }
  }
}

void Setup::restart(GameManager &gm) {
  if (gm.getRestart()) {
    gm.getEntityList().clear();
    Bullet::BulletList.clear();
    Bullet::ScheduledBulletList.clear();
    TextureManager::animationsVec.clear();
    Loot::ParticleVector.clear();
    Enemy::enemyRotations.clear();
    Loot::CoinVector.clear();

    Player player;
    EntityStats stats;
    stats.HP = player.HP;
    stats.BC = player.BC;
    stats.type = 0;
    stats.x = player.x;
    stats.y = player.y;
    stats.TextureID = player.TextureID;
    stats.src = player.srcR;
    stats.dest = player.dstR;
    stats.rotation = player.rotation;
    stats.velocity = player.velocity;
    stats.shooting_delay = player.shooting_delay;
    stats.effectTimer = 0;
    stats.UID = gm.generateEntityID(0);
    gm.getEntityManager().add(stats);
    gm.setRestart(false);
    gm.set_is_Paused(false);
    Sound::StopGameSound();
    Enemy::RestartWaves();

    gm.addScore((0.2 * Player::coin_pickup_played) +
                (10 * gm.getCurrentLevel()));
    auto it = std::lower_bound(ScoreHistory.begin(), ScoreHistory.end(),
                               gm.getTargetScore(),
                               [](int a, int b) { return a > b; });
    ScoreHistory.insert(it, gm.getTargetScore());

    gm.resetAllScore();
    gm.resetAllCoins();
    UI::DifficultyChosen = false;
    Sound::PlaySound(11);
    Player::PlayerUpgrades = PlayerUpgradeStats{};
    Map::MapGeneration();
    gm.setCurrentLevel(1);
    Map::NewMap(gm.getCurrentLevel());
  }
}

void Setup::nextLevel(GameManager &gm) {
  Bullet::BulletList.clear();
  Bullet::ScheduledBulletList.clear();
  TextureManager::animationsVec.clear();
  Loot::ParticleVector.clear();
  Enemy::enemyRotations.clear();
  Player::hitPlayerLevel = false;
  gm.set_is_Paused(true);
  UI::PushState(UIState::STORE_MENU);
  UI::Store(gm);
  Map::MapGeneration();
  gm.ppCurrentLevel();

  if (gm.getCurrentLevel() > 16) {
    gm.setRestart(true);
    Sound::PlaySound(11);
    restart(gm);
  }
  Sound::StopGameSound();
  Map::NewMap(gm.getCurrentLevel());
  Enemy::RestartWaves();
  Sound::PlaySound(11);
}

void Setup::update(GameManager &gm) {
  Player::playerInput(gm);
  Sound::PAndUNP_SoundEffects(true);

  if (gm.getRestart()) {
    restart(gm);
  }

  if (!gm.get_is_Paused()) {
    Sound::PAndUNP_SoundEffects(false);
    Sound::VolumeAdjustment();
    Player::AchievementLogging(gm);
    Enemy::SwarmingScenario();

    for (auto &Entity : gm.getEntityList()) {
      if (Entity.type == 0) {
        Entity.x += Entity.velocity.x * gm.getSpeed() * gm.getDelta() *
                    (Player::PlayerUpgrades.MovementSpeed);
        Entity.y += Entity.velocity.y * gm.getSpeed() * gm.getDelta() *
                    (Player::PlayerUpgrades.MovementSpeed);

        Entity.x = std::clamp(Entity.x, 0.0f, gm.getWindowW() - Entity.dest.w);
        Entity.y = std::clamp(Entity.y, 0.0f, gm.getWindowH() - Entity.dest.h);

        Entity.dest.x = Entity.x;
        Entity.dest.y = Entity.y;
      } else {
        Entity.x += Entity.velocity.x * gm.getSpeed() * gm.getDelta();
        Entity.y += Entity.velocity.y * gm.getSpeed() * gm.getDelta();
        Entity.dest.x = Entity.x;
        Entity.dest.y = Entity.y;
      }

      if (Entity.effectTimer > 0) {
        Entity.effectTimer -= gm.getDelta();
      }

      if (Entity.velocity.x != 0.0f || Entity.velocity.y != 0.0f) {
        bool found = false;

        SDL_FRect a = Entity.dest;
        float centerX = a.x + a.w / 2.0f;
        float centerY = a.y + a.h / 2.0f;

        if (Entity.type == 0) {
          a.h = a.h * 1.6;
          a.w = a.w * 1.6;
        }
        if (Entity.type == 1) {
          a.h = a.h * 2.0;
          a.w = a.w * 2.0;
        }
        if (Entity.type == 2) {
          a.w = a.w * 1.1;
          a.h = a.h * 1.8;
        }
        if (Entity.type == 3) {
          a.h = a.h * 1.5;
          a.w = a.w * 1.9;
        }
        if (Entity.type == 4) {
          a.h = a.h * 1.9;
          a.w = a.w * 2.1;
        }

        a.x = centerX - a.w / 2.0f;
        a.y = centerY - a.h / 2.0f;

        for (auto &animation : TextureManager::animationsVec) {
          if (animation.AnimationNumber == 8 + Entity.type &&
              animation.EUID == Entity.UID) {
            animation.destRect = a;
            animation.angle = Entity.rotation;
            found = true;
            break;
          }
        }

        if (!found) {
          TextureManager::animationsVec.emplace_back(
              0, 10, 300, Entity.rotation, 8 + Entity.type, a, Entity.UID);
        }
      } else {
        TextureManager::animationsVec.erase(
            std::remove_if(TextureManager::animationsVec.begin(),
                           TextureManager::animationsVec.end(),
                           [&](const AnimationVector &anim) {
                             return anim.AnimationNumber == 8 + Entity.type &&
                                    anim.EUID == Entity.UID;
                           }),
            TextureManager::animationsVec.end());
      }
    }

    Loot::UpdateCoins(gm);
    Loot::UpdateParticles(gm);
    TextureManager::AnimationCleaning();
    Enemy::EnemyAI(gm);
    Bullet::updateBullets(gm);
    Map::MapUpdate(gm);
    TextManager::ClearTextureCache();
  }
}

void Setup::render(GameManager &gm) {
  SDL_RenderClear(gm.getRenderer());
  Map::MapRender(gm);

  for (auto &animation : TextureManager::animationsVec) {
    if (animation.AnimationNumber >= 8) {
      TextureManager::DrawAnimation(animation, false, gm.getRenderer());
    }
  }

  for (auto &Entity : gm.getEntityList()) {
    if (Entity.TextureID >= 0 &&
        Entity.TextureID < TextureManager::TextureVec.size()) {
      if (Entity.effectTimer > 0) {
        SDL_SetTextureColorMod(TextureManager::TextureVec[Entity.TextureID],
                               255, 100, 100);
      } else {
        SDL_SetTextureColorMod(TextureManager::TextureVec[Entity.TextureID],
                               255, 255, 255);
      }
      TextureManager::DrawTextureNP(Entity.TextureID, gm.getRenderer(),
                                    &Entity.dest, Entity.rotation);
    } else {
      std::cerr << "Invalid TextureID: " << Entity.TextureID << std::endl;
    }
  }

  for (auto &Bullet : Bullet::BulletList) {
    TextureManager::DrawTexture(Bullet::textureBullet, gm.getRenderer(),
                                Bullet.srcR, Bullet.dstR, Bullet.rotation);
  }

  for (auto &animation : TextureManager::animationsVec) {
    if (animation.AnimationNumber >= 8) {
      continue;
    }
    bool colorMod = false;
    for (auto &Entity : gm.getEntityList()) {
      if ((animation.AnimationNumber == 5 && Entity.type == 3 &&
           Entity.effectTimer > 0) ||
          (animation.AnimationNumber == 7 && Entity.type == 4 &&
           Entity.effectTimer > 0)) {
        if (Entity.UID == animation.EUID) {
          colorMod = true;
        }
      }
    }
    TextureManager::DrawAnimation(animation, colorMod, gm.getRenderer());
  }

  for (auto &coin : Loot::CoinVector) {
    SDL_SetRenderDrawColor(gm.getRenderer(), 255, 255, 0, 255);
    SDL_RenderFillRect(gm.getRenderer(), &coin.dst);
    SDL_SetRenderDrawColor(gm.getRenderer(), 0, 0, 0, 255);
    SDL_RenderRect(gm.getRenderer(), &coin.dst);
  }

  for (auto &Part : Loot::ParticleVector) {
    SDL_SetRenderDrawColor(gm.getRenderer(), Part.color.r, Part.color.g,
                           Part.color.b, Part.color.a);
    SDL_RenderFillRect(gm.getRenderer(), &Part.dst);
    SDL_SetRenderDrawColor(gm.getRenderer(), 0, 0, 0, 255);
    SDL_RenderRect(gm.getRenderer(), &Part.dst);
  }

  UI::ScoreChange(gm);
  UI::CoinsChange(gm);
  UI::RenderUI(gm);
  SDL_RenderPresent(gm.getRenderer());
}

void Setup::quit() {
  GameManagerSingleton::instance().quit();
  SDL_Log("Cleaning up...");
  Sound::Cleanup();
  SDL_Quit();
  Bullet::BulletList.clear();
}
