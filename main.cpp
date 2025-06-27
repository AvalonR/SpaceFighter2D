#include <iostream>

#include "src/Enemy.h"
#include "src/Player.h"
#include "src/Setup.h"
#include "src/TextureManager.h"
Player player;
int main()
{
    freopen("console_log.txt", "w", stdout);
    freopen("console_log.txt", "a", stderr);
    Setup::initialization();
    Setup::EntityList.emplace_back(player.HP, player.BC, 0, player.x, player.y, 0, player.srcR, player.dstR, player.rotation, player.velocity, player.shooting_delay, 0, Setup::generateID(0));
    Enemy::SpawnEnemy(3);
    Enemy::SpawnEnemy(3);
    Enemy::SpawnEnemy(3);
    Enemy::SpawnEnemy(4);
    Setup::gameLoop();
    Setup::quit();
}
