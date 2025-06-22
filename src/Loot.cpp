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
std::vector<Particle> Loot::ParticleVector;

SDL_FRect particle = {0, 0, 8, 8};
void Loot::ParticleCreation(SDL_FPoint origin, SDL_Color color, int duration, int quantity) {
    for (int i = 0; i < quantity; i++)
    {
        particle.x = origin.x;
        particle.y = origin.y;
        Vector velocity;
        velocity.x = 0.2 * Enemy::Randomizer(-1, 1);
        velocity.y = 0.2 * Enemy::Randomizer(-2, 0);
        ParticleVector.emplace_back(particle, velocity, color, duration);
    }
}
void Loot::UpdateParticles() {
    for (auto& part : ParticleVector)
    {
        part.velocity.x = Map::lerp(part.velocity.x, 0, 0.01);
        part.velocity.y = Map::lerp(part.velocity.y, 0, 0.01);
        part.dst.x += part.velocity.x * Setup::speed;
        part.dst.y += part.velocity.y * Setup::speed;
        part.dst.w = part.dst.w * (1.0f - (part.lifetime * 0.0001));
        part.dst.h = part.dst.h * (1.0f - (part.lifetime * 0.0001));
        part.lifetime--; 
    }
    ParticleVector.erase(
        std::remove_if(ParticleVector.begin(), ParticleVector.end(),
                   [](const Particle& part) {
                       if (part.lifetime < 0) {
                            return true;
                       }
                       else {
                            return false;
                       }
                   }),
        ParticleVector.end());
}
