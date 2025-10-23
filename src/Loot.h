#pragma once
#include "Enemy.h"
#include "Map.h"
#include "Setup.h"
#include "Sound.h"
#include <utility>
struct CoinStructVec {
  SDL_FRect dst;
  Vector velocity;
  bool needErasing;
  int amount;
};

struct Particle {
  SDL_FRect dst;
  Vector velocity;
  SDL_Color color;
  float lifetime;
};

class Loot {
public:
  static std::vector<CoinStructVec> CoinVector;
  static std::vector<Particle> ParticleVector;
  static void spawnCoins(SDL_FRect position, int amount);
  static void UpdateCoins(GameManager &gm);
  static void ParticleCreation(SDL_FPoint origin, SDL_Color color, int duration,
                               int quantity);
  static void InvariableParticleCreation(SDL_FPoint origin, Vector velocity,
                                         SDL_Color color, int duration,
                                         int quantity);
  static void UpdateParticles(GameManager &gm);
};
