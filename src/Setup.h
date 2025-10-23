//
// Created by romak on 16.01.2025.
//

#ifndef SETUP_H
#define SETUP_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cstdint>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

struct Vector {
  float x, y;
};

struct EntityStats {
  float HP, BC, type;
  float x, y;
  int TextureID{};
  SDL_FRect src, dest;
  double rotation;
  Vector velocity;
  float shooting_delay, effectTimer;
  uint32_t UID;
};

// FOrward Declaration
class GameManager;

class CoinSystem {
private:
  int current;
  int target;
  int updateDelay;

public:
  CoinSystem() : current(0), target(0), updateDelay(0) {}

  void add(int amount) {
    if (amount > 0) {
      target += amount;
    }
  }

  bool spend(int cost) {
    if (current >= cost) {
      target -= cost;
      return true;
    }
    return false;
  }

  void reset() {
    current = 0;
    target = 0;
    updateDelay = 0;
  }

  int getCurrent() const { return current; }
  int getTarget() const { return target; }

  void animate() {
    if (updateDelay > 0) {
      --updateDelay;
      return;
    }

    int diff = target - current;
    if (diff == 0)
      return;

    int change = static_cast<int>(diff * 0.2f);
    if (change == 0) {
      change = (diff > 0 ? 1 : -1);
    }

    current += change;
    updateDelay = 7;
  }

  bool isAnimating() const { return current != target; }
  int pendingChange() const { return target - current; }
};

class ScoreSystem {
private:
  int current;
  int target;
  int updateDelay;
  float difficulty;

public:
  ScoreSystem() : current(-1), target(0), updateDelay(0), difficulty(1.0f) {}

  void add(int points) {
    if (points > 0) {
      target += static_cast<int>(points * difficulty);
    }
  }

  void reset() {
    current = -1;
    target = 0;
    updateDelay = 0;
  }

  int getCurrent() const { return current; }
  int getTarget() const { return target; }
  float getDifficulty() const { return difficulty; }

  void setDifficulty(float level) {
    if (level > 1.0f) {
      difficulty = level;
    }
  }

  void animate() {
    if (updateDelay > 0) {
      --updateDelay;
      return;
    }

    int change = static_cast<int>((target - current) * 0.2f);

    if (change == 0) {
      current = target;
    } else {
      current += change;
    }

    updateDelay = 7;
  }

  void keepPositive() {
    if (current < 0) {
      current = 0;
    }
  }

  bool isAnimating() const { return current < target; }
  int pendingChange() const { return target - current; }
};

class EntityManager {
private:
  std::vector<EntityStats> entities;
  std::mt19937 rng;
  std::unordered_set<uint32_t> usedIDs;

public:
  EntityManager() : rng(std::random_device{}()) {}

  uint32_t generateID(int TypeID) {
    if (TypeID < 0 || TypeID > 15) {
      throw std::invalid_argument("Type ID must be between 0 and 15");
    }

    std::uniform_int_distribution<uint32_t> dist{0, (1u << 28) - 1};
    uint32_t id;
    do {
      uint32_t base = dist(rng);
      id = (base << 4) | (TypeID & 0xF);
    } while (usedIDs.count(id));

    usedIDs.insert(id);
    return id;
  }

  void add(const EntityStats &entity) { entities.emplace_back(entity); }

  void clear() {
    entities.clear();
    usedIDs.clear();
  }

  std::vector<EntityStats> &getEntities() { return entities; }
  EntityStats &getPlayer() { return entities[0]; }
  const std::vector<EntityStats> &getEntities() const { return entities; }

  void removeDeadEntities(GameManager &gm);
};

class LevelManager {
private:
  int currentLevel;

public:
  LevelManager() : currentLevel(1) {}

  void setLevel(int level) {
    if (level > 0 && level < 17) {
      currentLevel = level;
    }
  }

  void nextLevel() {
    if (currentLevel < 16) {
      currentLevel++;
    }
  }

  int getLevel() const { return currentLevel; }

  void reset() { currentLevel = 1; }
};

// ============= END NEW MANAGERS =============

class GameManager {
private:
  // NEW: Internal systems
  CoinSystem coinSystem;
  ScoreSystem scoreSystem;
  EntityManager entityManager;
  LevelManager levelManager;

  SDL_Window *window;
  int WindowHeight, WindowWidth;
  SDL_Renderer *renderer;
  bool Vsync;
  int fps;

  float speed;
  double delta;

  bool loopRunning;
  bool is_Paused;
  bool Restart;
  float Max_FPS;

  SDL_Event event;
  Vector mouseCoordin;

public:
  void initializationSet();

  void reset() {
    coinSystem.reset();
    scoreSystem.reset();
    // entityManager.clear();
    levelManager.reset();
  }

  void quit() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
  }

  // ========== COIN SYSTEM WRAPPERS ==========
  void addCoins(int amount) { coinSystem.add(amount); }
  bool spendCoins(int cost) { return coinSystem.spend(cost); }
  void resetAllCoins() { coinSystem.reset(); }
  int getCoins() { return coinSystem.getCurrent(); }
  int getTargetCoins() { return coinSystem.getTarget(); }
  void animateCoinsStep() { coinSystem.animate(); }
  bool isAnimatingCoins() const { return coinSystem.isAnimating(); }
  int pendingCoinChange() const { return coinSystem.pendingChange(); }

  // Deprecated struct - keep for compatibility but discourage use
  struct CoinInfo {
    int current;
    int target;
    bool canAfford(int cost) const { return current >= cost; }
  };
  CoinInfo getCoinInfo() {
    return {coinSystem.getCurrent(), coinSystem.getTarget()};
  }

  // ========== SCORE SYSTEM WRAPPERS ==========
  void addScore(int points) { scoreSystem.add(points); }
  int getScore() { return scoreSystem.getCurrent(); }
  int getTargetScore() { return scoreSystem.getTarget(); }
  void resetAllScore() { scoreSystem.reset(); }
  void animateScoreStep() { scoreSystem.animate(); }
  void keepScore() { scoreSystem.keepPositive(); }
  void setDifficulty(float level) { scoreSystem.setDifficulty(level); }
  float getDifficulty() { return scoreSystem.getDifficulty(); }
  bool isAnimatingScore() const { return scoreSystem.isAnimating(); }
  int pendingScoreChange() const { return scoreSystem.pendingChange(); }

  // Deprecated struct
  struct ScoreInfo {
    int current;
    int target;
    float multiplier;
  };
  ScoreInfo getScoreInfo() {
    return {scoreSystem.getCurrent(), scoreSystem.getTarget(),
            scoreSystem.getDifficulty()};
  }

  // ========== ENTITY MANAGER WRAPPERS ==========
  std::vector<EntityStats> &getEntityList() {
    return entityManager.getEntities();
  }
  uint32_t generateEntityID(int typeID) {
    return entityManager.generateID(typeID);
  }

  // ========== LEVEL MANAGER WRAPPERS ==========
  int getCurrentLevel() { return levelManager.getLevel(); }
  void setCurrentLevel(int newLevel) { levelManager.setLevel(newLevel); }
  void ppCurrentLevel() { levelManager.nextLevel(); }

  // ========== DIRECT SYSTEM ACCESS (NEW API) ==========
  CoinSystem &getCoinsSystem() { return coinSystem; }
  ScoreSystem &getScoreSystem() { return scoreSystem; }
  EntityManager &getEntityManager() { return entityManager; }
  LevelManager &getLevelManager() { return levelManager; }

  // ========== INFRASTRUCTURE ==========
  int getWindowH() { return WindowHeight; }
  int getWindowW() { return WindowWidth; }
  SDL_Window *getWindow() { return window; }
  SDL_Renderer *getRenderer() { return renderer; }
  bool getVsync() { return Vsync; }
  float getSpeed() { return speed; }
  bool getloopRunning() { return loopRunning; }
  void setloopRunning(bool state) { loopRunning = state; }
  bool get_is_Paused() { return is_Paused; }
  void set_is_Paused(bool state) { is_Paused = state; }
  bool getRestart() { return Restart; }
  void setRestart(bool state) { Restart = state; }
  float getMax_FPS() { return Max_FPS; }
  SDL_Event &getEvent() { return event; }
  Vector &getMouseCoordin() { return mouseCoordin; }
  double getDelta() { return delta; }
  void setDelta(double newS) {
    if (newS >= 0) {
      delta = newS;
    }
  }
  void setFPS(int Fps) { fps = Fps; }
  int getFPS() { return fps; }
};

class GameManagerSingleton {
public:
  static GameManager &instance() {
    static GameManager instance;
    return instance;
  }
};

class Setup {
public:
  static const std::string basePath;
  static std::vector<int> ScoreHistory;
  static void initialization(GameManager &gm);
  static void gameLoop(GameManager &gm);
  static void restart(GameManager &gm);
  static void nextLevel(GameManager &gm);
  static void render(GameManager &gm);
  static void update(GameManager &gm);
  static void quit();
};

#endif // SETUP_H
