#include "Loot.h"
#include "Player.h"
#include <cmath>
#include <algorithm>

std::vector<CoinStructVec> Loot::CoinVector;

void Loot::spawnCoins(SDL_FRect position, int amount) {
    Vector velocity;
    velocity.x = 0.2 * Enemy::Randomizer(-1, 1);
    velocity.y = 0.2 * Enemy::Randomizer(-2, 0);
    CoinVector.emplace_back(position, velocity, false, amount);
    Sound::PlaySound(5);
}

void Loot::UpdateCoins() {
    for (auto& coin : CoinVector)
    {
        float diffX = (Setup::EntityList[0].x + Setup::EntityList[0].dest.w / 2) - coin.dst.x;
        float diffY = (Setup::EntityList[0].y + Setup::EntityList[0].dest.h / 2) - coin.dst.y;
        float dist = std::sqrt(diffX * diffX + diffY * diffY);
        if (dist > 21.0f && dist < 250.0f) {
            if (dist > 1.0f) {  
                diffX /= dist;
                diffY /= dist;
            }

            coin.velocity.x = Map::lerp(coin.velocity.x, diffX * 4.0f, 0.05f);
            coin.velocity.y = Map::lerp(coin.velocity.y, diffY * 4.0f, 0.05f);
        }
        else if (dist < 20.0f) { 
            coin.needErasing = true;
        }
        else {
            coin.velocity.x = Map::lerp(coin.velocity.x, 0, 0.01);
            coin.velocity.y = Map::lerp(coin.velocity.y, 0, 0.01);
        }
        coin.dst.x += coin.velocity.x * Setup::speed;
        coin.dst.y += coin.velocity.y * Setup::speed;
    }
    CoinVector.erase(
        std::remove_if(CoinVector.begin(), CoinVector.end(),
                   [](const CoinStructVec& coin) {
                       if (coin.needErasing) {
                            Setup::TargetCoins += coin.amount;
                            Player::coin_pickup_played++;
                            Sound::PlaySound(6);
                            return true;
                       }
                       else {
                            return false;
                       }
                   }),
        CoinVector.end());
}
