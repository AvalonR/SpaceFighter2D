//
// Created by romak on 18.01.2025.
//

#ifndef BULLET_H
#define BULLET_H

#include "Setup.h"
class Bullet {
public:
    Bullet();
    ~Bullet();

    float x{}, y{};
    SDL_FRect* srcR{};
    SDL_FRect* dstR{};
    int rotation{};
    Vector velocity{};
    int owner{};
    float damage{};

    static float bulletSpeed;
    static SDL_Texture* textureBullet;
    static std::vector<Bullet> BulletList;
    static std::vector<std::pair<Bullet, int>> ScheduledBulletList;
    static void spawnBulletPlayer();
    static void spawnBulletEnemy(Vector Entity_destxy, Vector Entity_destwh, double enemy_rotation);
    static void SpawnSpecificBullet(float entityCenterX, float entityCenterY, double angleRad, double angleRadians, int delayMS, float damage);
    static void updateBullets();
};



#endif //BULLET_H
