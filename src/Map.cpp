//
// Created by romak on 23.01.2025.
//

#include "Map.h"
#include "Enemy.h"
#include <iostream>
#include <memory>
#include <fstream>
#include <sstream>
#include <map>

#include "TextureManager.h"
std::vector<const char*> Map::MapTextureNames;
SDL_Texture* Map::MapTexture = nullptr;
SDL_FRect* Map::src = new SDL_FRect{0, 1920, 1920, 1080};
SDL_FRect* Map::dst = new SDL_FRect{0, 0, 1920, 1080};
int mapIndex = 0;
void Map::NewMap(int levelID) {
    MapTexture = nullptr;
    Enemy::Waves.clear();
    std::ifstream file("config.txt");
    std::string line;
    bool loadinglevel = false;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        if (line.rfind("L:", 0) == 0) {
            int currentlevel = std::stoi(line.substr(2));
            loadinglevel = (currentlevel == levelID);
        }

        if (!loadinglevel) continue;

        size_t mPos = line.find("M:");
        size_t wPos = line.find("W:");

        if (mPos != std::string::npos) {
            mapIndex = std::stoi(line.substr(mPos + 2));
            std::cout << mapIndex << std::endl;
            if (mapIndex >= 0 && mapIndex < MapTextureNames.size()) {
                MapTexture = TextureManager::LoadTexture(MapTextureNames[mapIndex]);
            }
        }

        if (wPos != std::string::npos) {
            std::string waveData = line.substr(wPos + 2); // until end of line
            std::stringstream waveStream(waveData);
            std::string waveEntry;

            int i = 0;
            while (std::getline(waveStream, waveEntry, '.')) {
                if (waveEntry.empty()) continue;

                std::stringstream entryStream(waveEntry);
                std::string typesStr, countsStr, delayStr;

                std::getline(entryStream, typesStr, ';');
                std::getline(entryStream, countsStr, ';');
                std::getline(entryStream, delayStr, ';');

                EnemyWave wave;
                std::sscanf(typesStr.c_str(), "%d,%d", &wave.EnemyTypes[0], &wave.EnemyTypes[1]);
                std::sscanf(countsStr.c_str(), "%d,%d", &wave.EnemyCount[0], &wave.EnemyCount[1]);
                wave.delayBetweenWaves = std::stoi(delayStr);
                wave.activated = false;

                if (i < 4) {
                    Enemy::basicDelay[i] = std::stoi(delayStr);
                }

                Enemy::Waves.push_back(wave);
                i++;
            }
        }
    }
    std::cout << "Config accepted for this level (" << levelID << ")" << std::endl;
}
void Map::MapGeneration() {
    std::ofstream configFile("config.txt");
    if (!configFile.is_open()) {
        std::cerr << "Failed to create Config File (config.txt)" << std::endl;
        return;
    }

    struct WaveData {
        std::vector<std::pair<int, int>> enemyTypesAndCounts; // {{type1, count1}, {type2, count2}, ...}
        int delay;
    };

    std::map<int, int> levelMapIndices;
    for (int i = 0; i < 6; i++) {
        levelMapIndices[i + 1] = Enemy::Randomizer(0, 5);
    }

    std::map<int, std::vector<WaveData>> levelWaves;

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 3; j++) {
            WaveData newWave;
            std::vector<std::pair<int, int>> enemies;
            if (i < 2) {
                if (j < 2) {
                    enemies.emplace_back(Enemy::Randomizer(1, 2), Enemy::Randomizer(1, 3));
                    enemies.emplace_back(Enemy::Randomizer(1, 2), Enemy::Randomizer(1, 3));
                } else {
                    enemies.emplace_back(3, 1);
                    enemies.emplace_back(Enemy::Randomizer(1, 2), 3 * Enemy::Randomizer(1, 3));
                }
            } else if (i < 4) {
                if (j < 2) {
                    enemies.emplace_back(Enemy::Randomizer(1, 2), 2 *Enemy::Randomizer(1, 3));
                    enemies.emplace_back(Enemy::Randomizer(1, 2), 2 *Enemy::Randomizer(1, 3));
                } else {
                    enemies.emplace_back(3, 1);
                    enemies.emplace_back(Enemy::Randomizer(1, 2), 3 * Enemy::Randomizer(1, 3));
                }
            } else {
                enemies.emplace_back(3, Enemy::Randomizer(1, 2));
                enemies.emplace_back(Enemy::Randomizer(1, 2), 3 * Enemy::Randomizer(1, 3));
            }

            newWave.enemyTypesAndCounts = enemies;
            newWave.delay = 1000 * Enemy::Randomizer(2, 4);
            levelWaves[i + 1].push_back(newWave); 
        }
    }

    for (const auto& [levelID, mapIndex] : levelMapIndices) {
        configFile << "L:" << levelID << "|";
        configFile << "M:" << mapIndex << "|";
        configFile << "W:";

        const auto& waves = levelWaves[levelID];
        for (size_t i = 0; i < waves.size(); ++i) {
            const auto& wave = waves[i];

            if (wave.enemyTypesAndCounts.size() >= 2) {
                configFile << wave.enemyTypesAndCounts[0].first << "," << wave.enemyTypesAndCounts[1].first << ";";
                configFile << wave.enemyTypesAndCounts[0].second << "," << wave.enemyTypesAndCounts[1].second << ";";
            } else {
                configFile << wave.enemyTypesAndCounts[0].first << ",0;";
                configFile << wave.enemyTypesAndCounts[0].second << ",0;";
            }

            configFile << wave.delay;
            if (i != waves.size() - 1)
                configFile << ".";
        }

        configFile << "|\n";
    }

    configFile.close();
    std::cout << "config.txt written.\n";
}
void Map::MapUpdate()
{
    src->y = src->w = static_cast<float>(Setup::WindowWidth);
    src->h = static_cast<float>(Setup::WindowHeight);
    float x_right = 600, y_bottom = 0;

    src->x = lerp(src->x, Setup::EntityList[0].dest.x - x_right, 0.05f);
    src->y = lerp(src->y, Setup::EntityList[0].dest.y - y_bottom, 0.05f);

    if (src->x < 0)
    {
        src->x = 0;
    }
    if (src->y < 0)
    {
        src->y = 0;
    }
    if (src->x > 1080) 
    {
        src->x = 1080; 
    }
    if (src->y > 920) 
    {
        src->y = 920; 
    }
}
float Map::lerp(float start, float end, float t)
{
    return start + t * (end - start);
}
void Map::MapRender()
{
    if (MapTexture == nullptr)
    {
        MapTexture = TextureManager::LoadTexture(MapTextureNames[mapIndex]);
    }
    TextureManager::DrawMap(MapTexture, Setup::renderer, src, dst);
}

