#include "Setup.h"
#include "Enemy.h"
#include "Map.h"
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
    int lifetime;
};

class Loot {
public:
    static std::vector<CoinStructVec> CoinVector;
    static std::vector<Particle> ParticleVector;
    static void spawnCoins(SDL_FRect position, int amount);  
    static void UpdateCoins();
    static void ParticleCreation(SDL_FPoint origin, SDL_Color color, int duration, int quantity);
    static void UpdateParticles();
};

