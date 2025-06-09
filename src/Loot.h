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
class Loot {
public:
    static std::vector<CoinStructVec> CoinVector;
    static void spawnCoins(SDL_FRect position, int amount);  
    static void UpdateCoins();
};

