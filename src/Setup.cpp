#include "Setup.h"

#include <cstdint>
#include <iostream>
#include <ostream>

#include "Bullet.h"
#include "EnemyNew.h"
#include "Entity.h"
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

  Player *player = new Player();
  player->setUID(gm.generateEntityID(0));
  gm.getEntityManager().add(player);

  gm.reset();

  Entity *enemy = Enemy::SpawnEnemy(3);
  if (enemy) {
    gm.getEntityManager().add(enemy);
  }
}

void Setup::gameLoop(GameManager &gm) {
  unsigned int a = SDL_GetTicks();
  unsigned int b = SDL_GetTicks();
  unsigned int fpsTimer = SDL_GetTicks();
  int frames = 0;
  int fps = 0;

  const float REFERENCE_DELTA = 1000.0f / 144.0f;

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
    gm.getEntityManager().clear();
    Bullet::BulletList.clear();
    Bullet::ScheduledBulletList.clear();
    TextureManager::animationsVec.clear();
    Loot::ParticleVector.clear();
    Enemy::enemyRotations.clear();
    Loot::CoinVector.clear();

    Player *player = new Player();
    player->setUID(gm.generateEntityID(0));
    gm.getEntityManager().add(player);

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

    Entity *player = gm.getEntityManager().getPlayer();
    for (Entity *entity : gm.getEntityManager().getEntities()) {
      if (!entity)
        continue;

      entity->update(gm);

      if (entity->getTypeID() != 0 && player) {
        Enemy *enemy = static_cast<Enemy *>(entity);
        enemy->updateAI(gm, *player, gm.getDelta());
      }
    }

    Loot::UpdateCoins(gm);
    Loot::UpdateParticles(gm);
    TextureManager::AnimationCleaning();

    Bullet::updateBullets(gm);

    gm.getEntityManager().removeDeadEntities(gm);

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

  for (Entity *entity : gm.getEntityManager().getEntities()) {
    if (!entity)
      continue;

    int textureID = entity->getTextureID();
    if (textureID >= 0 && textureID < TextureManager::TextureVec.size()) {
      if (entity->getEffectTimer() > 0) {
        SDL_SetTextureColorMod(TextureManager::TextureVec[textureID], 255, 100,
                               100);
      } else {
        SDL_SetTextureColorMod(TextureManager::TextureVec[textureID], 255, 255,
                               255);
      }
      SDL_FRect dest = entity->getDestRect();
      TextureManager::DrawTextureNP(textureID, gm.getRenderer(), &dest,
                                    entity->getRotation());
    } else {
      std::cerr << "Invalid TextureID: " << textureID << std::endl;
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
    for (Entity *entity : gm.getEntityManager().getEntities()) {
      if (!entity)
        continue;
      int entityType = entity->getTypeID();
      if ((animation.AnimationNumber == 5 && entityType == 3 &&
           entity->getEffectTimer() > 0) ||
          (animation.AnimationNumber == 7 && entityType == 4 &&
           entity->getEffectTimer() > 0)) {
        if (entity->getUID() == animation.EUID) {
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
