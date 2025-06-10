//
// Created by romak on 18.01.2025.
//

#include "Enemy.h"

#include <algorithm>
#include <cmath>
#include <random>

#include "Bullet.h"
#include "Player.h"
#include "Setup.h"
#include "Sound.h"
#include "TextureManager.h"
#include "UI.h"
#include "Loot.h"
float angle = 0.0f; // Angle in radians, used for circular movement
float radius = 400.0f; // Distance from the player
float rotationSpeed = 0.1f;
std::vector<int> enemyRotations;

Enemy::Enemy()
{
    HP = 1.0f;
    BC = 1.0f;
    x = y = 0.0;
    srcR = {};
    dstR = {0, 0, 64, 60};
    TextureID = 1;
    velocity.x = velocity.y = 0;
    rotation = 0;
    shooting_delay = 0;
    velocity.x = velocity.y = 0;
    type = 0;
}

Enemy::~Enemy() = default;


int Enemy::basicDelay[4] = {3000, 4000, 5000, 6000};
std::vector<EnemyWave> Enemy::Waves; 

int currentWaveIndex = 0; // Tracks which wave we're on
void Enemy::RestartWaves()
{
    currentWaveIndex = 0;
    for (int t = 0; t < Waves.size(); t++)
    {
        Waves[t].activated = false;
        Waves[t].delayBetweenWaves = basicDelay[t];
    }
}
int aclock = 0;

void Enemy::SwarmingScenario()
{
    if (currentWaveIndex >= Waves.size()) {
        if (Setup::EntityList.size() < 2) {
            Player::finished_level = true;
            if (aclock > 500) {
                aclock = 0;
                Setup::nextLevel();
            } else {
                aclock++;
            }
        }
        return;
    }
    EnemyWave& currentWave = Waves[currentWaveIndex]; 

    if (!currentWave.activated)
    {
        for (int t = 0; t < 2; t++) {
            if (currentWave.EnemyCount[t] > 0 && currentWave.EnemyTypes[t] > 0) {
                for (int j = 0; j < currentWave.EnemyCount[t]; j++) {
                    SpawnEnemy(currentWave.EnemyTypes[t]);
                }
            }
        }
        currentWave.activated = true;
    }
    else
    {
        if (currentWave.delayBetweenWaves > 0)
        {
            currentWave.delayBetweenWaves--;
        }
        else
        {
            Player::hitPlayerWave = false;
            currentWaveIndex++;
        }
    }
}

static constexpr Vector SpawnPositions[] = {
    {-400, -400},
    {960, -400},
    {2320, -400},
    {-400, 1280},
    {960, 1580},
    {2320, 1280},
    {-400, 540},
    {2320, 540}
};

int Enemy::Randomizer(int x, int y)
{
    std::random_device rd;
    std::uniform_int_distribution<int> distrib(x, y);
    return distrib(rd);
}

void Enemy::SpawnEnemy(int type)
{
    Enemy enemy;
    enemy.x = SpawnPositions[Randomizer(0, 7)].x;
    enemy.y = SpawnPositions[Randomizer(0, 7)].y;
    if (enemy.x > 0 && enemy.x < static_cast<float>(Setup::WindowWidth) && enemy.y > 0 && enemy.y < static_cast<float>(
        Setup::WindowHeight))
    {
        // Force the spawn position to be off-screen
        if (Randomizer(0, 1) == 0)
            enemy.x = (Randomizer(0, 1) == 0) ? -200 : Setup::WindowWidth + 200;
        else
            enemy.y = (Randomizer(0, 1) == 0) ? -200 : Setup::WindowWidth + 200;
    }

    enemy.dstR.x = enemy.x;
    enemy.dstR.y = enemy.y;
    enemy.dstR.h = enemy.dstR.h = 64;
    if (type == 1)
    {
        enemy.dstR.w = 64;
        enemy.dstR.h = 60;
    }
    if (type == 2)
    {
        enemy.dstR.w = 84;
        enemy.dstR.h = 64;
    }
    if (type == 3)
    {
        enemy.HP = 3.0f;
        enemy.dstR.x = 200;
        enemy.dstR.y = 200;
        enemy.dstR.w = 68 * 2;
        enemy.dstR.h = 98 * 2;
        enemy.rotation = 180;
        enemy.shooting_delay = 1200;
    }

    Setup::EntityList.emplace_back(enemy.HP, enemy.BC, type, enemy.x, enemy.y, type, enemy.srcR, enemy.dstR,
                                   enemy.rotation, enemy.velocity, enemy.shooting_delay);
    enemyRotations.emplace_back((Randomizer(0, 1) == 0) ? 1 : -1);
}

void Enemy::EnemyAI()
{
    Setup::EntityList.erase(std::ranges::remove_if(Setup::EntityList, [](EntityStats& Entity)
    {
        if (Entity.HP <= 0 && Entity.type == 0)
        {
            RestartWaves();
            Setup::Restart = true;
            UI::UIAction = 2;
        }
        if (Entity.HP <= 0 && Entity.type != 0 )
        {
            SDL_FRect a = {
                Entity.dest.x + Entity.velocity.x * Setup::speed, Entity.dest.y + Entity.velocity.y * Setup::speed,
                Entity.dest.w, Entity.dest.h
            };
            SDL_FRect coinrec = {
                Entity.dest.x + Entity.velocity.x * Setup::speed, Entity.dest.y + Entity.velocity.y * Setup::speed,
                8, 8
            };
            if (Entity.type == 1)
            {
                a.h = a.h * 2;
                a.w = a.w * 2;
                TextureManager::animationsVec.emplace_back(0, 20, 300, Entity.rotation, 2, a);
                Setup::TargetScore += 20;
                Sound::PlaySound(9);
                Player::PlayerUpgrades.ExperienceP = std::min(Player::PlayerUpgrades.ExperienceP + 0.15f, 1.0f);
                Player::enemies_killed++;
                Loot::spawnCoins(coinrec, 10);
            }
            if (Entity.type == 2 && Entity.HP != -0.9999f)
            {
                a.w = a.w * 1.2;
                a.h = (a.h + 20) * 1.2;
                TextureManager::animationsVec.emplace_back(0, 20, 300, Entity.rotation, 0, a);
                Setup::TargetScore += 25;
                Sound::PlaySound(9);
                Player::PlayerUpgrades.ExperienceP = std::min(Player::PlayerUpgrades.ExperienceP + 0.1f, 1.0f);
                Player::enemies_killed++;
                Loot::spawnCoins(coinrec, 15);
            }
            if (Entity.type == 2 && Entity.HP == -0.9999f)
            {
                a.w = a.w * 1.2;
                a.h = (a.h + 20) * 1.2;
                TextureManager::animationsVec.emplace_back(0, 20, 300, Entity.rotation, 0, a);
            } 
            if (Entity.type == 3)
            {
                a.x = Entity.x;
                a.y = Entity.y;
                a.w = 256;
                a.h = 256;
                for (auto& animation : TextureManager::animationsVec) {
                    if (animation.AnimationNumber == 6 || animation.AnimationNumber == 4 || animation.AnimationNumber == 5) {
                        animation.expiration = -1;
                    }
                }
                TextureManager::animationsVec.emplace_back(0, 20, 300, Entity.rotation, 3, a);
                Sound::PlaySound(9);
                Player::PlayerUpgrades.ExperienceP = std::min(Player::PlayerUpgrades.ExperienceP + 0.3f, 1.0f);
                Setup::TargetScore += 250;
                Player::enemies_killed++;
                Player::bosses_killed++;
                Loot::spawnCoins(coinrec, 100);
            }
            return true;
        } 
        return false;
    }).begin(), Setup::EntityList.end());
    for (auto& Entity : Setup::EntityList)
    {
        float forceX = 0.0f, forceY = 0.0f;

        if (Entity.type == 1) // Circling Enemy (Bullet Hell)
        {
            // Compute attraction to player
            float dx = Setup::EntityList[0].dest.x - Entity.x;
            float dy = Setup::EntityList[0].dest.y - Entity.y;
            float distance = std::sqrt(dx * dx + dy * dy);

            if (distance > radius)
            {
                forceX += (dx / distance) * 0.5f;
                forceY += (dy / distance) * 0.5f;
            }

            // Repulsion from other enemies
            for (auto& OtherEnemy : Setup::EntityList)
            {
                if (&Entity == &OtherEnemy) continue;

                float diffX = Entity.x - OtherEnemy.x;
                float diffY = Entity.y - OtherEnemy.y;
                float dist = std::sqrt(diffX * diffX + diffY * diffY);

                if (dist < 100 && dist > 40) // Smooth repulsion
                {
                    float repelForce = 0.3f / dist;
                    forceX += (diffX / dist) * repelForce;
                    forceY += (diffY / dist) * repelForce;
                }
                else if (dist <= 40)
                {
                    // Soft randomization to prevent stacking
                    forceX += -1.5f * static_cast<float>(Randomizer(-1, 1));
                    forceY += -1.5f * static_cast<float>(Randomizer(-1, 1));
                }
            }
            float orbitDirection;
            size_t index = &Entity - &Setup::EntityList[0];
            if (index - 1 < enemyRotations.size())
            {
                orbitDirection = static_cast<float>(enemyRotations[index-1]);
            }
            else
            {
                std::cerr << "Index " << index << " out of bounds for enemyRotations (size: " << enemyRotations.size() << ")\n";
                orbitDirection = static_cast<float>(60);
            }

            float angle = std::atan2(dy, dx);
            float orbitForceX = std::sin(angle) * 0.5f * orbitDirection;
            float orbitForceY = -std::cos(angle) * 0.5f * orbitDirection;

            forceX += orbitForceX;
            forceY += orbitForceY;

            // Enemy Shooting
            Entity.rotation = (std::atan2(dy, dx) * (180.0f / M_PI)) + 90;

            if (Entity.shooting_delay <= 0)
            {
                Bullet::spawnBulletEnemy({Entity.dest.x, Entity.dest.y}, {Entity.dest.w, Entity.dest.h},
                                         Entity.rotation);
                Entity.shooting_delay = 200;
            }
            else
            {
                Entity.shooting_delay -= 1;
            }

            // Apply forces using smooth interpolation
            Entity.velocity.x = Map::lerp(Entity.velocity.x, forceX, 0.1f);
            Entity.velocity.y = Map::lerp(Entity.velocity.y, forceY, 0.1f);
        }

        // **TYPE 2 - Seeker Enemy (Suicide Bomber)**
        if (Entity.type == 2)
        {
            float dx = Setup::EntityList[0].dest.x - Entity.x;
            float dy = Setup::EntityList[0].dest.y - Entity.y;
            float distance = std::sqrt(dx * dx + dy * dy);

            if (distance > 20) // Faster approach
            {
                forceX += (dx / distance) * 0.7f;
                forceY += (dy / distance) * 0.7f;
            }

            Entity.rotation = std::atan2(dy, dx) * (180.0f / M_PI) + 90;

            // Explosion on contact
            if (SDL_HasRectIntersectionFloat(&Setup::EntityList[0].dest, &Entity.dest))
            {
                SDL_FRect result;
                SDL_GetRectIntersectionFloat(&Setup::EntityList[0].dest, &Entity.dest, &result);

                result.w = std::max(Entity.dest.w, result.w);
                result.h = std::max(Entity.dest.h, result.h);

                TextureManager::animationsVec.emplace_back(0, 20, 200, 0, 1, result);
                Entity.HP = -0.9999f;
                Setup::EntityList[0].HP -= (0.2f * Setup::Difficulty);
                Setup::EntityList[0].effectTimer = 40;
                Player::hitPlayerLevel = true;
                Player::hitPlayerWave = true;
                float angle = std::atan2(Entity.dest.y - result.y, Entity.dest.x - result.x);
                Setup::EntityList[0].velocity.x = Map::lerp(Setup::EntityList[0].velocity.x,
                                                            Setup::EntityList[0].velocity.x - std::cos(angle) * 0.4,
                                                            0.1f); // Knockback effect
                Setup::EntityList[0].velocity.y = Map::lerp(Setup::EntityList[0].velocity.y,
                                                            Setup::EntityList[0].velocity.y - std::sin(angle) * 0.4,
                                                            0.1f); // Knockback effect
                Sound::PlaySound(2);
            }

            // Repulsion from other enemies
            for (auto& OtherEnemy : Setup::EntityList)
            {
                if (&Entity == &OtherEnemy) continue;

                float diffX = Entity.x - OtherEnemy.x;
                float diffY = Entity.y - OtherEnemy.y;
                float dist = std::sqrt(diffX * diffX + diffY * diffY);

                if (dist < 100 && dist > 40)
                {
                    float repelForce = 0.3f / dist;
                    forceX += (diffX / dist) * repelForce;
                    forceY += (diffY / dist) * repelForce;
                }
                else if (dist <= 40)
                {
                    forceX += -1.5f * static_cast<float>(Randomizer(-1, 1));
                    forceY += -1.5f * static_cast<float>(Randomizer(-1, 1));
                }
            }

            // Smooth movement
            Entity.velocity.x = Map::lerp(Entity.velocity.x, forceX, 0.2f);
            Entity.velocity.y = Map::lerp(Entity.velocity.y, forceY, 0.2f);
        }
        if (Entity.type == 3)
        {
            Entity.x = Map::lerp(Entity.x, Setup::EntityList[0].x, 0.008f);
            if (Entity.y <= 80 || Entity.y >= 120)
            {
                Entity.y = Map::lerp(Entity.y, 100, 0.02f);
                float dx = Setup::EntityList[0].dest.x - Entity.x;
                float dy = 100 - Entity.y;

                Entity.rotation = Map::lerp(Entity.rotation, std::atan2(dy, dx) * (180.0f / M_PI) + 180, 0.2f);
            }
            else
            {
                Entity.rotation = Map::lerp(Entity.rotation, 180, 0.02f);
            }
            Entity.dest.x = Entity.x;
            Entity.dest.y = Entity.y;
            if (Entity.shooting_delay == 400)
            {
                SDL_FRect shipanim = {Entity.dest.x - Entity.dest.w / 2, Entity.dest.y - Entity.dest.h / 2, 256, 256};
                TextureManager::animationsVec.emplace_back(0, 20, 800, 180, 6, shipanim);
                Sound::PlaySound(7);
            }
            if (Entity.shooting_delay <= 0)
            {
                SDL_FRect a = {Entity.x + Entity.dest.w / 2 - 128, Entity.y + Entity.dest.h, 256, 256};
                TextureManager::animationsVec.emplace_back(0, 1, 450, 0, 4, a);
                a.y += 256;
                TextureManager::animationsVec.emplace_back(0, 1, 450, 0, 5, a);
                a.y += 256;
                TextureManager::animationsVec.emplace_back(0, 1, 450, 0, 5, a);
                Sound::PlaySound(8);
                Entity.shooting_delay = 1200;
            }
            for (int t = 0; t < TextureManager::animationsVec.size(); t++)
            {
                if (TextureManager::animationsVec[t].AnimationNumber == 4 || TextureManager::animationsVec[t].
                    AnimationNumber == 5)
                {
                    TextureManager::animationsVec[t].destRect.x = Entity.x + Entity.dest.w / 2 - 128;
                    SDL_FRect LaserHitBox = {
                        TextureManager::animationsVec[t].destRect.x + TextureManager::animationsVec[t].destRect.w / 3,
                        TextureManager::animationsVec[t].destRect.y,
                        TextureManager::animationsVec[t].destRect.w - TextureManager::animationsVec[t].destRect.w * 2 /
                        3,
                        TextureManager::animationsVec[t].destRect.h
                    };
                    bool hitSomething = false;
                    for (auto& enemy : Setup::EntityList)
                    {
                        if (enemy.type != 3 && SDL_HasRectIntersectionFloat(&enemy.dest, &LaserHitBox))
                        {
                            SDL_FRect Intersection;
                            SDL_GetRectIntersectionFloat(&enemy.dest, &LaserHitBox, &Intersection);
                            TextureManager::animationsVec[t].destRect.h = Intersection.y - TextureManager::animationsVec
                                [t].destRect.y;


                            enemy.HP = Map::lerp(enemy.HP, enemy.HP - (0.15f * Setup::Difficulty), 0.02f);
                            enemy.effectTimer = 15;
                            hitSomething = true;
                            break;
                        }
                    }
                    if (t > 0)
                    {
                        if (TextureManager::animationsVec[t - 1].AnimationNumber == 4 || TextureManager::animationsVec[t - 1].AnimationNumber == 5)
                        {
                            if (TextureManager::animationsVec[t - 1].destRect.h < 255)
                            {
                                TextureManager::animationsVec[t].destRect.h = 0;
                            }
                        }
                    }
                    if (!hitSomething)
                    {
                        bool canRestore = true;

                        if (t > 0)
                        {
                            if (TextureManager::animationsVec[t - 1].AnimationNumber == 4 ||
                                TextureManager::animationsVec[t - 1].AnimationNumber == 5)
                            {
                                if (TextureManager::animationsVec[t - 1].destRect.h < 255)
                                // Small threshold to avoid flickering
                                {
                                    canRestore = false;
                                }
                            }
                        }

                        if (canRestore)
                        {
                            TextureManager::animationsVec[t].destRect.h = Map::lerp(
                                TextureManager::animationsVec[t].destRect.h,
                                256,
                                0.1f
                            );
                        }
                    }
                }

                if (TextureManager::animationsVec[t].AnimationNumber == 6)
                {
                    TextureManager::animationsVec[t].destRect.x = Entity.dest.x - Entity.dest.w / 2.3;
                    TextureManager::animationsVec[t].destRect.y = Entity.dest.y - Entity.dest.h / 5.8;
                    TextureManager::animationsVec[t].angle = Entity.rotation;
                }
            }
            Entity.shooting_delay--;
        }
    }
}
