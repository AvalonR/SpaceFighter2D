//
// Created by romak on 18.01.2025.
//

#include "Bullet.h"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "Loot.h"
#include "Enemy.h"
#include "Sound.h"
#include "Player.h"
#include "TextureManager.h"

SDL_Texture* Bullet::textureBullet = nullptr;
std::vector<Bullet> Bullet::BulletList = {};
float Bullet::bulletSpeed = 8.0f;

Bullet::Bullet()
{
    x = y = 0;
    srcR = new SDL_FRect{};
    dstR = new SDL_FRect{0, 0, 64, 64};
    rotation = 0;
    velocity = {0, 0};
    owner = -1;
    damage = 0;
}

Bullet::~Bullet() = default;
std::vector<std::pair<Bullet, int>> Bullet::ScheduledBulletList;

void Bullet::SpawnSpecificBullet(float entityCenterX, float entityCenterY, double angleRad, double angleRadians, int delayMS, float damage) {
        float spawnDistance = 100.0f;  
        Bullet bullet;
        bullet.dstR->x = entityCenterX + cos(angleRad) * spawnDistance - bullet.dstR->w / 2;
        bullet.dstR->y = entityCenterY + sin(angleRad) * spawnDistance - bullet.dstR->h / 2;
        bullet.dstR->w = bullet.dstR->h = 64;

        bullet.x = bullet.dstR->x;
        bullet.y = bullet.dstR->y;

        // Calculate velocity components based on rotation
        bullet.velocity.x = -bulletSpeed * std::cos(angleRadians);
        bullet.velocity.y = -bulletSpeed * std::sin(angleRadians);
        if (bullet.dstR->x < -100 || bullet.dstR->y < -100 || bullet.dstR->x > 2020 || bullet.dstR->y > 1920)
        {
            // SDL_Log("Error: Bullet spawned with invalid coordinates!");
        }
        else
        {
            bullet.owner = 0;
            bullet.damage = damage + Player::PlayerUpgrades.damagePerBullet;
            if (delayMS > 0) {
                ScheduledBulletList.emplace_back(std::pair<Bullet, int>(bullet, delayMS));
            }
            else {
                BulletList.emplace_back(bullet);
            }
            Sound::PlaySound(0);
        }
}

void Bullet::spawnBulletPlayer()
{

        float entityCenterX = Setup::EntityList[0].dest.x + Setup::EntityList[0].dest.w / 2;
        float entityCenterY = Setup::EntityList[0].dest.y + Setup::EntityList[0].dest.h / 2;
        double angleRad = (Setup::EntityList[0].rotation - 90) * (M_PI / 180.0f);

    if (Player::PlayerUpgrades.firemodes[0]) { //Shotgun fire
        int bulletcount = 2 + Player::PlayerUpgrades.extraBulletsPerShot;
        float spread = 0.2f;  
        
        for (int i = 0; i < bulletcount; i++) {
            double angleRadians = ((Setup::EntityList[0].rotation + 90) * (M_PI / 180.0f)) ;
            float offset = spread * (i - (bulletcount - 1) / 2.0f);
            SpawnSpecificBullet(entityCenterX, entityCenterY, angleRad + offset, angleRadians + offset, 0, (0.2f / (bulletcount)));
        }
    }
    if (Player::PlayerUpgrades.firemodes[1]) { //Radial fire
        int bulletcount = 8 + Player::PlayerUpgrades.extraBulletsPerShot;
        
        for (int i = 0; i < bulletcount; i++) {
            float angle = (2 * M_PI / bulletcount) * i;
            SpawnSpecificBullet(entityCenterX, entityCenterY, angleRad, angle, 10 * i, (0.2f / (bulletcount)));
        }
    }
    if (Player::PlayerUpgrades.firemodes[2]) { //Parallel fire
        float spacing = 50.0f;
        int bulletcount = 2 + Player::PlayerUpgrades.extraBulletsPerShot;
        
        for (int i = 0; i < bulletcount; ++i) {
            float sideOffset = spacing * (i - (bulletcount - 1) / 2.0f);
            float offsetX = cos(angleRad + M_PI_2) * sideOffset;
            float offsetY = sin(angleRad + M_PI_2) * sideOffset;
            double angleRadians = (Setup::EntityList[0].rotation + 90) * (M_PI / 180.0f);
            SpawnSpecificBullet(entityCenterX + offsetX, entityCenterY + offsetY, angleRad, angleRadians, 0, (0.2f / (bulletcount))); 
        }
    }
    if (!Player::PlayerUpgrades.firemodes[0] && !Player::PlayerUpgrades.firemodes[1] && !Player::PlayerUpgrades.firemodes[2]) {
        int bulletcount = 1 + Player::PlayerUpgrades.extraBulletsPerShot;

        for (int i = 0; i < bulletcount; i++) {
            double angleRadians = (Setup::EntityList[0].rotation + 90) * (M_PI / 180.0f);
            SpawnSpecificBullet(entityCenterX, entityCenterY, angleRad, angleRadians, 10 * i, (0.2f / (bulletcount)));
        }
    }
}

void Bullet::spawnBulletEnemy(Vector Entity_destxy, Vector Entity_destwh, double enemy_rotation)
{
    Bullet bullet;

    double angleRadians = (enemy_rotation-90) * (M_PI / 180.0f);

    float spawnDistance = 80.0f;  

    float entityCenterX = Entity_destxy.x + Entity_destwh.x / 2;
    float entityCenterY = Entity_destxy.y + Entity_destwh.y / 2;
    bullet.dstR->x = entityCenterX + cos(angleRadians) * spawnDistance - bullet.dstR->w / 2;  // Subtract half the bullet's width to center it
    bullet.dstR->y = entityCenterY + sin(angleRadians) * spawnDistance - bullet.dstR->h / 2;  // Subtract half the bullet's height to center it
    bullet.dstR->w = bullet.dstR->h = 64;

    bullet.x = bullet.dstR->x;
    bullet.y = bullet.dstR->y;

    bullet.velocity.x = bulletSpeed * std::cos(angleRadians);
    bullet.velocity.y = bulletSpeed * std::sin(angleRadians);

    if (bullet.dstR->x < 0 || bullet.dstR->y < 0 || bullet.dstR->x > 1920 || bullet.dstR->y > 1920)
    {
        // SDL_Log("Error: Bullet spawned with invalid coordinates!");
    }
    else
    {
        bullet.owner = 1;
        bullet.damage = (0.05f * Setup::Difficulty);
        BulletList.emplace_back(bullet);
        Sound::PlaySound(1);
    }
}
SDL_FRect BulletParticle;
void Bullet::updateBullets()
{
    if (!ScheduledBulletList.empty()) {
        for (auto& scheduled: ScheduledBulletList) {
            if (scheduled.second < 0) {
                BulletList.emplace_back(scheduled.first);
                scheduled.second = -9999;
            }
            else {
                scheduled.second--;
            }
        }
        ScheduledBulletList.erase(
        std::remove_if(ScheduledBulletList.begin(), ScheduledBulletList.end(),
                       [](std::pair<Bullet, int>& scheduled) { return scheduled.second <= -9999; }),
        ScheduledBulletList.end());
    }
    BulletList.erase(
        std::remove_if(BulletList.begin(), BulletList.end(),
                       [](Bullet& bullet)
                       {
                           int index = 0;
                           for (auto& Entity : Setup::EntityList)
                           {
                               index++;
                               SDL_FRect EntitycollisionRect = {Entity.dest.x * 1.01f, Entity.dest.y * 1.01f, Entity.dest.w * 0.95f,Entity.dest.w * 0.95f};
                               SDL_FRect BulletcollisionRect = {bullet.dstR->x * 1.05f, bullet.dstR->y * 1.05f, bullet.dstR->w * 0.5f,bullet.dstR->w * 0.5f};
                               if (SDL_GetRectIntersectionFloat(&BulletcollisionRect, &EntitycollisionRect, &BulletParticle) && bullet.owner != Entity.type)
                               {
                                   Entity.HP -= bullet.damage;
                                   Entity.effectTimer = 25;
                                   Sound::PlaySound(3);
                                    if (bullet.owner == 0) {
                                        Player::shots_landed++;
                                    }
                                   if (Entity.type == 0)
                                   {
                                        Player::hitPlayerWave = true;
                                        Player::hitPlayerLevel = true;
                                   }
                                   if (Entity.type != 3)
                                   {
                                       float angle = std::atan2(Entity.dest.y - bullet.dstR->y, Entity.dest.x - bullet.dstR->x);
                                       Entity.velocity.x = Map::lerp(Entity.velocity.x, std::cos(angle) * 0.2, 0.1f);
                                       Entity.velocity.y = Map::lerp(Entity.velocity.y, std::sin(angle) * 0.2, 0.1f);
                                   }
                                    Sound::PlaySound(13);
                                    SDL_FPoint spark = {BulletParticle.x, BulletParticle.y};
                                    Loot::ParticleCreation(spark, {255, 155, 0, 255}, 300, 3);
                                   return true;
                               }
                           }
                           if (bullet.dstR == nullptr)
                           {
                               std::cout << "Bullet with wrong coordinates detected" << std::endl;
                               return true;
                           }
                           bullet.x += bullet.velocity.x;
                           bullet.y += bullet.velocity.y;
                           bullet.dstR->x = bullet.x;
                           bullet.dstR->y = bullet.y;
                           bullet.rotation += 1;

                           if ((bullet.dstR->y < 0 || bullet.dstR->y > 1080) || (bullet.dstR->x < 0 || bullet.dstR->x >
                               1920))
                           {
                               return true; 
                           }
                           return false; 
                       }),
        BulletList.end()
    );
}
