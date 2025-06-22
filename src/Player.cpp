//
// Created by romak on 16.01.2025.
//

#include "Player.h"
#include <cmath>
#include <iostream>
#include <ostream>
#include <unordered_map>
#include <bits/stl_algo.h>

#include "Bullet.h"
#include "Sound.h"
#include "UI.h"

bool Player::LeftMouse = false;
bool Player::RightMouse = false;
std::unordered_map<SDL_Keycode, bool> keyState; // Holds the state of keys

Player::Player()
{
    HP = 1.0f;
    BC = 1.0f;
    x = y = 800.0;
    srcR = {};
    dstR = {800, 500, 84, 84};
    TextureID = 0;
    velocity.x = velocity.y = 0;
    rotation = 0;
    shooting_delay = 0;
}

Player::~Player() = default;
int PlayerReloadingCooldown = 0;
bool isReloading = false;
int Player::enemies_killed = 0;
int Player::bosses_killed = 0;
int Player::shots_landed = 0;
int Player::coin_pickup_played = 0;
bool Player::finished_level = false;
bool Player::upgradedPlayer = false;
bool Player::hitPlayerWave = false;
bool Player::hitPlayerLevel = false;
PlayerUpgradeStats Player::PlayerUpgrades;
Achieve Player::Achievements[15];
int completedachivements = 0;
void Player::AchievementLogging() {
    Achievements[0].title = "First Blood";
    if (enemies_killed == 1 && !Achievements[0].achieved) {
        Achievements[0].achieved = true;
        Achievements[0].description = "Took down your very first enemy. The war begins.";
        completedachivements++;
    }
    Achievements[1].title = "Coming In Hot";
    if (enemies_killed == 10 && !Achievements[1].achieved) {
        Achievements[1].achieved = true;
        Achievements[1].description = "You're not just lucky — you're getting good. 10 foes down.";
        completedachivements++;
    }
    Achievements[2].title = "Wrecking Machine";
    if (enemies_killed == 100 && !Achievements[2].achieved) {
        Achievements[2].achieved = true;
        Achievements[2].description = "Left a hundred enemies in ruins. And you're just warming up.";
        completedachivements++;
    }
    Achievements[3].title = "Boss Slayer";
    if (bosses_killed == 1 && !Achievements[3].achieved) {
        Achievements[3].achieved = true;
        Achievements[3].description = "You've slain your first boss. That roar? It was your own.";
        completedachivements++;
    }
    Achievements[4].title = "Boss Massacre";
    if (bosses_killed == 10 && !Achievements[4].achieved) {
        Achievements[4].achieved = true;
        Achievements[4].description = "Ten bosses fell before your might. Are you even mortal?";
        completedachivements++;
    }
    Achievements[5].title = "Treasure Hunter";
    if (Setup::TargetCoins >= 100 && !Achievements[5].achieved) {
        Achievements[5].achieved = true;
        Achievements[5].description = "Collected 100 coins. Wealth favors the brave.";
        completedachivements++;
    }
    Achievements[6].title = "Sound of Gold";
    if (coin_pickup_played >= 100 && !Achievements[6].achieved) {
        Achievements[6].achieved = true;
        Achievements[6].description = "You've heard it enough to dream in coins. 100 pickups later, still satisfying..";
        completedachivements++;
    }
    Achievements[7].title = "Survivor's Instinct";
    if (finished_level && Setup::EntityList[0].HP < 0.3f && !Achievements[7].achieved) {
        Achievements[7].achieved = true;
        Achievements[7].description = "Barely made it out alive. But alive is alive.";
        completedachivements++;
    }
    Achievements[8].title = "Wanderer of the Stars";
    if (Setup::CurrentLevel >= 3 && !Achievements[8].achieved) {
        Achievements[8].achieved = true;
        Achievements[8].description = "No boundary is too distant for a curious soul. Visited many a level";
        completedachivements++;
    }
    Achievements[9].title = "Dodger Supreme";
    if (!hitPlayerWave && !Achievements[9].achieved) {
        Achievements[9].achieved = true;
        Achievements[9].description = "Not a scratch. You dance between bullets like wind.";
        completedachivements++;
    }
    Achievements[10].title = "Armed and Ready";
    if (upgradedPlayer && !Achievements[10].achieved) {
        Achievements[10].achieved = true;
        Achievements[10].description = "You’ve reached most serious firepower. Let them come.";
        completedachivements++;
    }
    Achievements[11].title = "Laser Surgeon";
    if (shots_landed > 100 && !Achievements[11].achieved) {
        Achievements[11].achieved = true;
        Achievements[11].description = "Your aim cuts cleaner than steel.";
        completedachivements++;
    }
    Achievements[12].title = "Untouchable";
    if (!hitPlayerLevel && finished_level && !Achievements[12].achieved) {
        Achievements[12].achieved = true;
        Achievements[12].description = "They tried. They missed. Every single one.";
        completedachivements++;
    }
    Achievements[13].title = "Wavebreaker";
    if (finished_level && !Achievements[13].achieved) {
        Achievements[13].achieved = true;
        Achievements[13].description = "No wave too wild. You crushed them all.";
        completedachivements++;
    }
    Achievements[14].title = "Completionist";
    if (completedachivements == 14) {
        Achievements[14].achieved = true;
        Achievements[14].description = "Every feat, every challenge — complete. You're legendary.";
    }
}
int healingcooldown = 144;
void Player::playerInput()
{
    if (PlayerUpgrades.ExperienceP == 1.0f) {
        Sound::PlaySound(10);
        UI::UIAction = 5;
        UI::CurrentLayer = 4;
        Setup::is_Paused = true;
        UI::LevelUp();
    }
    if (!Setup::is_Paused){
        healingcooldown--;
        PlayerReloadingCooldown--;
    }
    if (Setup::EntityList[0].HP < 1.0f + Player::PlayerUpgrades.bonusTotalHP && healingcooldown < 0) {
        if (!Setup::is_Paused){
            Setup::EntityList[0].HP = std::min(Setup::EntityList[0].HP * (Player::PlayerUpgrades.PassiveHealing), 1.0f + Player::PlayerUpgrades.bonusTotalHP);
            healingcooldown = 144;
        }
    }
    if (Setup::EntityList[0].BC >= 1.0f + PlayerUpgrades.extraBulletCap)
    {
        isReloading = false;
    }
    if (isReloading == true && PlayerReloadingCooldown <= 0)
    {
        if (!Setup::is_Paused){
            Sound::PlaySound(14);
            Setup::EntityList[0].BC += 0.1f;
            PlayerReloadingCooldown = static_cast<int>(72 * PlayerUpgrades.bulletCooldownMultiplier);
        }
    }
    Setup::EntityList[0].shooting_delay--;
    //player rotation according to mousepos
    SDL_GetGlobalMouseState(&Setup::mouseCoordin.x, &Setup::mouseCoordin.y);
    // SDL_GetMouseState(&Setup::mouseCoordin.x, &Setup::mouseCoordin.y);
    // Calculate deltas using double precision
    double deltaX = static_cast<double>(Setup::mouseCoordin.x) - static_cast<double>(Setup::EntityList[0].dest.x +
        Setup::EntityList[0].dest.w / 2);
    double deltaY = static_cast<double>(Setup::mouseCoordin.y) - static_cast<double>(Setup::EntityList[0].dest.y +
        Setup::EntityList[0].dest.h / 2);

    // Calculate the angle in radians and convert to degrees
    double angleDegrees = (std::atan2(deltaY, deltaX) * (180.0f / M_PI)) + 90.0;

    while (SDL_PollEvent(&Setup::event))
    {
        if (Setup::event.type == SDL_EVENT_QUIT)
        {
            Setup::loopRunning = false;
            return;
        }

        if (Setup::event.type == SDL_EVENT_KEY_DOWN)
        {
            keyState[Setup::event.key.key] = true;

            if (Setup::event.key.key == SDLK_ESCAPE)
            {
                if (UI::UIAction == 0)
                {
                    UI::UIAction = 1;
                }
                else if (UI::UIAction == 4) 
                {

                } 
                else
                {
                    UI::UIAction = 0;
                }
            }
        }
        if (Setup::event.type == SDL_EVENT_MOUSE_WHEEL) {
            UI::Scrolling -= (Setup::event.wheel.y * 10);
        }
        if (Setup::event.type == SDL_EVENT_KEY_UP)
        {
                for(int i = 0; i < 7; i++) {
                UI::ControlsPressed[i] = false;
                }
            keyState[Setup::event.key.key] = false;
        }
        Setup::EntityList[0].velocity.x = 0.0f;
        Setup::EntityList[0].velocity.y = 0.0f;
        if (keyState[SDLK_E])
        {
            UI::ControlsPressed[2] = true;
            if (Setup::is_Paused != true)
            {
                if (Setup::EntityList[0].shooting_delay <= 0 && Setup::EntityList[0].BC >= 0)
                {
                    isReloading = false;
                    Setup::EntityList[0].shooting_delay = 100;
                    Setup::EntityList[0].BC -= 0.1f;
                    Bullet::spawnBulletPlayer();
                }
            }
        }
        if (keyState[SDLK_Q])
        {
            UI::ControlsPressed[0] = true;
            if (Setup::is_Paused != true)
            {
                if (Setup::EntityList[0].BC < 1.0f && !isReloading)
                {
                    isReloading = true;
                }
            }
        }
        if (keyState[SDLK_W])
        {
            UI::ControlsPressed[1] = true;
        }
        if (keyState[SDLK_S])
        {
            UI::ControlsPressed[4] = true;
        }
        if (keyState[SDLK_A])
        {
            UI::ControlsPressed[3] = true;
        }
        if (keyState[SDLK_D])
        {
            UI::ControlsPressed[5] = true;
        }
        if (Setup::event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
        {
            if (Setup::event.button.button == SDL_BUTTON_LEFT)
            {
                LeftMouse = true;
            }
            else if (Setup::event.button.button == SDL_BUTTON_RIGHT)
            {
                RightMouse = true;
            }
        }
        if (Setup::event.type == SDL_EVENT_MOUSE_BUTTON_UP)
        {
            if (Setup::event.button.button == SDL_BUTTON_LEFT)
            {
                LeftMouse = false;
            }
            else if (Setup::event.button.button == SDL_BUTTON_RIGHT)
            {
                RightMouse = false;
            }
        }
        if (Setup::is_Paused) return;
        Setup::EntityList[0].rotation = angleDegrees;

        if (keyState[SDLK_W]) { Setup::EntityList[0].velocity.y -= 1.0f; UI::ControlsPressed[1] = true;}// Move up
        if (keyState[SDLK_S]) { Setup::EntityList[0].velocity.y += 1.0f; UI::ControlsPressed[4] = true;}// Move down
        if (keyState[SDLK_A]) { Setup::EntityList[0].velocity.x -= 1.0f; UI::ControlsPressed[3] = true;}// Move left
        if (keyState[SDLK_D]) { Setup::EntityList[0].velocity.x += 1.0f; UI::ControlsPressed[5] = true;}// Move right
        Setup::EntityList[0].x = std::clamp(Setup::EntityList[0].x, 0.0f,
                                            Setup::WindowWidth - Setup::EntityList[0].dest.w);
        Setup::EntityList[0].y = std::clamp(Setup::EntityList[0].y, 0.0f,
                                            Setup::WindowHeight - Setup::EntityList[0].dest.h);
        // Apply friction to prevent infinite bouncing
        Setup::EntityList[0].velocity.x *= 0.9f;
        Setup::EntityList[0].velocity.y *= 0.9f;

        // Update SDL Rect position
        Setup::EntityList[0].dest.x = Setup::EntityList[0].x;
        Setup::EntityList[0].dest.y = Setup::EntityList[0].y;
        Setup::EntityList[0].dest.x = std::clamp(Setup::EntityList[0].dest.x, 0.0f,
                                                 Setup::WindowWidth - Setup::EntityList[0].dest.w);
        Setup::EntityList[0].dest.y = std::clamp(Setup::EntityList[0].dest.y, 0.0f,
                                                 Setup::WindowHeight - Setup::EntityList[0].dest.h);
    }
}
