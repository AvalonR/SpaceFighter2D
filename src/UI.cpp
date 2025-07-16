//
// Created by romak on 18.01.2025.
//

#include "UI.h"

#include <iostream>
#include <map>
#include <cstring>
#include <cmath>

#include "Enemy.h"
#include "Player.h"
#include "TextManager.h"
#include "TextureManager.h"
#include "Sound.h"

int UI::UIAction;
SDL_Texture* UI::UITexture = nullptr;
SDL_Texture* UI::FullEscapeTexture = nullptr;
SDL_Texture* UI::NoButtonEscapeTexture = nullptr;
const char* UI::CurrentMenu = nullptr;
SDL_Texture* MainMenuBackground = nullptr;

std::multimap<int, const char*> ButtonLayout;
int UI::CurrentLayer = 1;
int buttonCooldown = 0;
int UI::Scrolling = 0;
bool UI::ControlsPressed[7];

int LevelPopUp = -1;

void UI::Init()
{
    UITexture = SDL_CreateTexture(Setup::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1920, 1080);
    if (!UITexture)
    {
        SDL_Log("Failed to create UI texture! SDL_Error: ", SDL_GetError());
    }

    SDL_SetTextureBlendMode(UITexture, SDL_BLENDMODE_BLEND);
    CurrentMenu = "Main_Menu";
    MainMenuBackground = TextureManager::LoadTexture("background1 .png");
    ButtonLayout.insert(std::pair<int, const char*>(1, "Swarm Mode"));
    ButtonLayout.insert(std::pair<int, const char*>(1, "Restart Swarm Mode"));
    ButtonLayout.insert(std::pair<int, const char*>(1, "Score Board"));
    ButtonLayout.insert(std::pair<int, const char*>(1, "Achievements"));
    ButtonLayout.insert(std::pair<int, const char*>(1, "Quit"));
    ButtonLayout.insert(std::pair<int, const char*>(1, "Settings"));
    ButtonLayout.insert(std::pair<int, const char*>(2, "Video"));
    ButtonLayout.insert(std::pair<int, const char*>(2, "Audio"));
    ButtonLayout.insert(std::pair<int, const char*>(2, "Misc"));
    ButtonLayout.insert(std::pair<int, const char*>(2, "Back"));
    ButtonLayout.insert(std::pair<int, const char*>(2, "Main Menu"));
    ButtonLayout.insert(std::pair<int, const char*>(3, "Main Menu"));
    ButtonLayout.insert(std::pair<int, const char*>(3, "Back"));
    ButtonLayout.insert(std::pair<int, const char*>(3, "1920x1080"));
    ButtonLayout.insert(std::pair<int, const char*>(3, "1280x720"));
    ButtonLayout.insert(std::pair<int, const char*>(3, "--------"));
    ButtonLayout.insert(std::pair<int, const char*>(3, "FullScreen"));
    ButtonLayout.insert(std::pair<int, const char*>(3, "Windowed"));
    ButtonLayout.insert(std::pair<int, const char*>(4, "Restart"));
    ButtonLayout.insert(std::pair<int, const char*>(4, "Main Menu"));
    ButtonLayout.insert(std::pair<int, const char*>(4, "Novice (1x)"));
    ButtonLayout.insert(std::pair<int, const char*>(4, "Veteran (1.5x)"));
    ButtonLayout.insert(std::pair<int, const char*>(4, "Calamity (2x)"));
    ButtonLayout.insert(std::pair<int, const char*>(4, "Extremity (3x)"));
    ButtonLayout.insert(std::pair<int, const char*>(4, "Next Level"));
    ButtonLayout.insert(std::pair<int, const char*>(4, "Heal +20 Health"));
    ButtonLayout.insert(std::pair<int, const char*>(4, "Increase Max"));
    ButtonLayout.insert(std::pair<int, const char*>(4, "Boost Max"));
    ButtonLayout.insert(std::pair<int, const char*>(4, "Upgrade Bullet"));
    ButtonLayout.insert(std::pair<int, const char*>(4, "Multi-Shot:"));
    ButtonLayout.insert(std::pair<int, const char*>(4, "Unlock Shotgun"));
    ButtonLayout.insert(std::pair<int, const char*>(4, "Unlock Radial Fire"));
    ButtonLayout.insert(std::pair<int, const char*>(4, "Unlock Parallel"));
    ButtonLayout.insert(std::pair<int, const char*>(4, "Battle-Hardened"));
    ButtonLayout.insert(std::pair<int, const char*>(4, "Sharpened Aim"));
    ButtonLayout.insert(std::pair<int, const char*>(4, "Efficient Repair"));
    ButtonLayout.insert(std::pair<int, const char*>(4, "Thruster"));
    ButtonLayout.insert(std::pair<int, const char*>(5, "Back"));
}
int buttongoDown = 0;
bool UI::DifficultyChosen = false;

void UI::RenderUI()
{
    SDL_SetRenderTarget(Setup::renderer, UITexture);
    buttonCooldown--;

    SDL_SetRenderDrawBlendMode(Setup::renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(Setup::renderer, 0, 0, 0, 0); 
    SDL_RenderClear(Setup::renderer);
    // std::cout << CurrentMenu;
    if (CurrentMenu != nullptr)
    {
        if (std::strcmp(CurrentMenu, "Main_Menu") == 0)
        {
            SDL_RenderTexture(Setup::renderer, MainMenuBackground, nullptr, nullptr);
            if (Button({855, 500, 180, 35}, {138, 43, 226, 180}, "Swarm Mode", {860, 500}, 24))
            {
                CurrentMenu = "Swarm_Mode";
                if (!DifficultyChosen)
                {
                    UIAction = 7;
                }
            }
            if (Setup::Score > 0)
            {
                if (Button({805, 600, 290, 35}, {138, 43, 226, 180}, "Restart Swarm Mode", {810, 600}, 24))
                {
                    Enemy::RestartWaves();
                    Setup::Restart = true;
                    CurrentMenu = "Swarm_Mode";
                    UIAction = 7;
                }
            }
            if (Button({855, 920, 180, 35}, {138, 43, 226, 180}, "Quit", {860, 920}, 24))
            {
                Setup::loopRunning = false;
                return;
            }
            if (Button({855, 750, 200, 35}, {138, 43, 226, 180}, "Score Board", {860, 750}, 24) || UIAction == 5)
            {
                UIAction = 5;
                CurrentLayer = 5;
                ScoreBoard();
            }
            if (Button({855, 800, 200, 35}, {138, 43, 226, 180}, "Achievements", {860, 800}, 24) || UIAction == 6)
            {
                UIAction = 6;
                CurrentLayer = 5;
                Achievements();
            }
            if (Button({855, 850, 180, 35}, {138, 43, 226, 180}, "Settings", {860, 850}, 24) || UIAction == 1)
            {
                UIAction = 1;
                CurrentLayer = 2;
                Escape();
            }
            if (UIAction == 0)
            {
                CurrentLayer = 1;
            }
            SDL_FRect Buttons = {1600, 880, 64, 64};
            if (ControlsPressed[0] ) {
                TextureManager::DrawTextureNP(27, Setup::renderer, &Buttons, 0); 
            }
            else {
                TextureManager::DrawTextureNP(28, Setup::renderer, &Buttons, 0); 
            }
            Buttons = {1660, 880, 64, 64};
            if (ControlsPressed[1]) {
                TextureManager::DrawTextureNP(31, Setup::renderer, &Buttons, 0); 
            }
            else {
                TextureManager::DrawTextureNP(32, Setup::renderer, &Buttons, 0); 
            }
            Buttons = {1720, 880, 64, 64};
            if (ControlsPressed[2]) {
                TextureManager::DrawTextureNP(25, Setup::renderer, &Buttons, 0); 
            }
            else {
                TextureManager::DrawTextureNP(26, Setup::renderer, &Buttons, 0); 
            }
            Buttons = {1610, 940, 64, 64};
            if (ControlsPressed[3]) {
                TextureManager::DrawTextureNP(35, Setup::renderer, &Buttons, 0); 
            }
            else {
                TextureManager::DrawTextureNP(36, Setup::renderer, &Buttons, 0); 
            }
            Buttons = {1670, 940, 64, 64};
            if (ControlsPressed[4]) {
                TextureManager::DrawTextureNP(33, Setup::renderer, &Buttons, 0); 
            }
            else {
                TextureManager::DrawTextureNP(34, Setup::renderer, &Buttons, 0); 
            }
            Buttons = {1730, 940, 64, 64};
            if (ControlsPressed[5]) {
                TextureManager::DrawTextureNP(37, Setup::renderer, &Buttons, 0); 
            }
            else {
                TextureManager::DrawTextureNP(38, Setup::renderer, &Buttons, 0); 
            }
            Buttons = {1810, 910, 64, 64};
            TextureManager::DrawTextureNP(39, Setup::renderer, &Buttons, 0); 
        }
        if (std::strcmp(CurrentMenu, "Swarm_Mode") == 0)
        {
            PlayerStats();
            if (LevelPopUp >= 0)
            {
                NextLevelPopUp();
            }
            if (UIAction == 2)
            {
                CurrentLayer = 4;
                Setup::is_Paused = true;
                RestartRun();
            }
            if (UIAction == 4)
            {
                CurrentLayer = 4;
                Setup::is_Paused = true;
                Store();
            }
            if (UIAction == 5)
            {
                CurrentLayer = 4;
                Setup::is_Paused = true;
                LevelUp();
            }
            if (UIAction == 7)
            {
                CurrentLayer = 4;
                Setup::is_Paused = true;
                DifficultyChoice();
            }
            if (UIAction == 0)
            {
                Setup::is_Paused = false;
                CurrentLayer = 1;
            }
            if (UIAction == 1)
            {
                CurrentLayer = 2;
                Setup::is_Paused = true;
                Escape();
            }
        }
    }

    SDL_SetRenderTarget(Setup::renderer, nullptr);
    SDL_RenderTexture(Setup::renderer, UITexture, nullptr, nullptr);
    if (CurrentMenu == "Main_Menu")
    {
        SDL_RenderPresent(Setup::renderer);
    }
}

void UI::PlayerStats()
{
    if (Player::PlayerUpgrades.TotalHP > 1.0f) {
        int numberOfSectors = std::max(1, static_cast<int>(ceil(Player::PlayerUpgrades.TotalHP)));
        int sizeOfSectors = 200 / numberOfSectors;
        
        float hp = std::clamp(Setup::EntityList[0].HP, 0.0f, static_cast<float>(numberOfSectors));
        
        SDL_FRect HPbar = {1700, 1040, 200, 20};
        SDL_FRect InnerHPBar = {1700, 1040, 200 * (hp / numberOfSectors), 20};
        
        SDL_SetRenderDrawColor(Setup::renderer, 70, 70, 70, 235);
        SDL_RenderFillRect(Setup::renderer, &HPbar);
        SDL_SetRenderDrawColor(Setup::renderer, 255, 0, 0, 255);
        SDL_RenderRect(Setup::renderer, &HPbar);
        SDL_RenderFillRect(Setup::renderer, &InnerHPBar);
        SDL_SetRenderDrawColor(Setup::renderer, 0, 0, 0, 255);
        for (int i = 1; i < numberOfSectors; i++) {
            SDL_RenderLine(Setup::renderer, 1700 + (sizeOfSectors * i), 1042, 1700 + (sizeOfSectors * i), 1058);
        }
    } else {
        SDL_FRect HPbar = {1700, 1040, 200, 20};
        SDL_FRect InnerHPBar = {1700, 1040, 200 * Setup::EntityList[0].HP, 20};
        SDL_SetRenderDrawColor(Setup::renderer, 70, 70, 70, 235);
        SDL_RenderFillRect(Setup::renderer, &HPbar);
        SDL_SetRenderDrawColor(Setup::renderer, 255, 0, 0, 255);
        SDL_RenderRect(Setup::renderer, &HPbar);
        SDL_RenderFillRect(Setup::renderer, &InnerHPBar);
    }
    if (Player::PlayerUpgrades.TotalBulletCap > 1.0f) {
        int numberOfBulletSectors = std::max(1, static_cast<int>(ceil(Player::PlayerUpgrades.TotalBulletCap)));
        int sizeOfBulletSectors = 200 / numberOfBulletSectors;
        
        float bc = std::clamp(Setup::EntityList[0].BC, 0.0f, static_cast<float>(numberOfBulletSectors));

        SDL_FRect BulletCap = {220, 1040, -200, 20};
        SDL_FRect InnerBulletCap = {220, 1040, -200 * (bc / numberOfBulletSectors), 20};

        SDL_SetRenderDrawColor(Setup::renderer, 70, 70, 70, 235);
        SDL_RenderFillRect(Setup::renderer, &BulletCap);
        SDL_SetRenderDrawColor(Setup::renderer, 255, 255, 0, 255);
        SDL_RenderRect(Setup::renderer, &BulletCap);
        SDL_RenderFillRect(Setup::renderer, &InnerBulletCap);
        SDL_SetRenderDrawColor(Setup::renderer, 0, 0, 0, 255);
        for (int i = 1; i < numberOfBulletSectors; ++i) {
            SDL_RenderLine(Setup::renderer, 220 - (sizeOfBulletSectors * i), 1042, 220 - (sizeOfBulletSectors * i), 1058);
        }
    } else {
        SDL_FRect BulletCap = {220, 1040, -200, 20};
        SDL_FRect InnerBulletCap = {220, 1040, -200 * Setup::EntityList[0].BC, 20};

        SDL_SetRenderDrawColor(Setup::renderer, 70, 70, 70, 235);
        SDL_RenderFillRect(Setup::renderer, &BulletCap);
        SDL_SetRenderDrawColor(Setup::renderer, 255, 255, 0, 255);
        SDL_RenderRect(Setup::renderer, &BulletCap);
        SDL_RenderFillRect(Setup::renderer, &InnerBulletCap);
    }
    SDL_FRect EXPbar = {20, 10, 1880, 30};
    SDL_FRect InnerEXPbar = {20, 10, 1880 * Player::PlayerUpgrades.ExperienceP, 30};

    SDL_SetRenderDrawColor(Setup::renderer, 70, 70, 70, 235);
    SDL_RenderFillRect(Setup::renderer, &EXPbar);
    SDL_SetRenderDrawColor(Setup::renderer, 0, 255, 255, 255);
    SDL_RenderRect(Setup::renderer, &EXPbar);
    SDL_RenderFillRect(Setup::renderer, &InnerEXPbar);

    std::string score = "Score: " + std::to_string(Setup::Score);
    TextManager::RenderText(score.c_str(), {640, 60}, {255, 255, 255, 255}, 24);

    std::string coins = "Coins: " + std::to_string(Setup::Coins);
    TextManager::RenderText(coins.c_str(), {1040, 60}, {255, 255, 255, 255}, 24);

    std::string entityCount = "Enemies: " + std::to_string(Setup::EntityList.size() - 1);
    TextManager::RenderText(entityCount.c_str(), {1740, 60}, {255, 255, 255, 255}, 24);

    SDL_SetRenderDrawColor(Setup::renderer, 0, 0, 0, 255);
    SDL_FRect HPIcon = {230, 1032, 32, 32};
    TextureManager::DrawTextureNP(21, Setup::renderer, &HPIcon, 0); 
    SDL_FRect BCIcon = {1662, 1032, 32, 32};
    TextureManager::DrawTextureNP(12, Setup::renderer, &BCIcon, 0); 
}

bool video_settings = false;
bool audio_settings = false;
bool misc_settings = false;

void UI::RestartRun()
{
    SDL_FRect RestartOutline = {700, 400, 500, 250};
    SDL_SetRenderDrawColor(Setup::renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(Setup::renderer, &RestartOutline);
    SDL_SetRenderDrawColor(Setup::renderer, 0, 255, 255, 255);
    SDL_RenderRect(Setup::renderer, &RestartOutline);
    if (Button({865, 450, 155, 35}, {138, 43, 226, 180}, "Restart", {870, 450}, 24))
    {
        Setup::RandomizeMusic();
        Setup::Restart = true;
        UIAction = 0;
        CurrentLayer = 1;
        UIAction = 6;
    }
    if (Button({865, 550, 155, 35}, {138, 43, 226, 180}, "Main Menu", {870, 550}, 24))
    {
        Setup::Restart = true;
        UIAction = 0;
        CurrentLayer = 1;
        CurrentMenu = "Main_Menu";
    }
}

void UI::DifficultyChoice()
{
    SDL_FRect RestartOutline = {700, 400, 500, 400};
    SDL_SetRenderDrawColor(Setup::renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(Setup::renderer, &RestartOutline);
    SDL_SetRenderDrawColor(Setup::renderer, 0, 255, 255, 255);
    SDL_RenderRect(Setup::renderer, &RestartOutline);
    if (Button({845, 430, 205, 35}, {138, 43, 226, 180}, "Novice (1x)", {850, 430}, 24, {255, 255, 255, 255}))
    {
        Setup::Difficulty = 1.0f;
        UIAction = 0;
        CurrentLayer = 1;
        DifficultyChosen = true;
        LevelPopUp++;
    }
    if (Button({845, 530, 205, 35}, {138, 43, 226, 180}, "Veteran (1.5x)", {850, 530}, 24, {255, 200, 200, 255}))
    {
        Setup::Difficulty = 1.5f;
        UIAction = 0;
        CurrentLayer = 1;
        DifficultyChosen = true;
        LevelPopUp++;
    }
    if (Button({845, 630, 205, 35}, {138, 43, 226, 180}, "Calamity (2x)", {850, 630}, 24, {255, 120, 120, 255}))
    {
        Setup::Difficulty = 2.0f;
        UIAction = 0;
        CurrentLayer = 1;
        DifficultyChosen = true;
        LevelPopUp++;
    }
    if (Button({845, 730, 205, 35}, {138, 43, 226, 180}, "Extremity (3x)", {850, 730}, 24, {255, 0, 0, 255}))
    {
        Setup::Difficulty = 3.0f;
        UIAction = 0;
        CurrentLayer = 1;
        DifficultyChosen = true;
        LevelPopUp++;
    }
}

void UI::NextLevelPopUp()
{
    if (LevelPopUp >= 250)
    {
        LevelPopUp = -1;
        return;
    }
    LevelPopUp++;
    SDL_FRect PopUPOutline = {700, 440, 500, 120};
    SDL_SetRenderDrawColor(Setup::renderer, 0, 0, 0, 250 - LevelPopUp);
    SDL_RenderFillRect(Setup::renderer, &PopUPOutline);
    SDL_SetRenderDrawColor(Setup::renderer, 0, 255, 255, 255 - LevelPopUp);
    SDL_RenderRect(Setup::renderer, &PopUPOutline);
    std::string text = "Current Level: " + std::to_string(Setup::CurrentLevel);
    TextManager::RenderText(text.c_str(), {820, 480}, {255, 255, 255, static_cast<Uint8>(255 - LevelPopUp)}, 28);
}
void UI::Achievements() {
    const int entryHeight = 60;
    const int maxVisibleEntries = 5; 
    const int totalEntries = 15;
    SDL_FRect SettingsOutline = {500, 300, 900, 500};
    SDL_SetRenderDrawColor(Setup::renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(Setup::renderer, &SettingsOutline);
    SDL_SetRenderDrawColor(Setup::renderer, 255, 255, 255, 255);
    SDL_RenderRect(Setup::renderer, &SettingsOutline);
    int firstIndex = Scrolling / entryHeight;
    int lastIndex = std::min(firstIndex + maxVisibleEntries, totalEntries);

    int maxScroll = std::max(0, (totalEntries - maxVisibleEntries) * entryHeight);
    Scrolling = std::clamp(Scrolling, 0, maxScroll);
    SDL_FRect rectAround = {590, 300, 750, 55};
    for (int i = firstIndex; i < lastIndex; i++) {
        float yOffset = 350.0f - (Scrolling % entryHeight) + ((i - firstIndex) * entryHeight);
        rectAround.y = yOffset;

        if (Player::Achievements[i].achieved) {
            SDL_RenderRect(Setup::renderer, &rectAround);
            std::string score = std::to_string(i + 1) + " " + Player::Achievements[i].title;
            TextManager::RenderText(score.c_str(), {600, yOffset}, {255, 255, 255, 255}, 24);
            TextManager::RenderText(Player::Achievements[i].description.c_str(), {600, yOffset + 25}, {255, 255, 255, 255}, 24);
        } else {
            SDL_RenderRect(Setup::renderer, &rectAround);
            std::string questionedTitle(Player::Achievements[i].title.length(), '?');
            std::string score = std::to_string(i + 1) + " " + questionedTitle;
            TextManager::RenderText(score.c_str(), {600, yOffset}, {255, 255, 255, 255}, 24);
        }
    if (totalEntries > maxVisibleEntries) {
        float scrollbarHeight = SettingsOutline.h * ((float)maxVisibleEntries / totalEntries);
        float scrollbarY = SettingsOutline.y + (float(Scrolling) / maxScroll) * (SettingsOutline.h - scrollbarHeight);

        SDL_FRect scrollbar = {
            SettingsOutline.x + SettingsOutline.w - 10,
            scrollbarY,
            6,
            scrollbarHeight
        };

        SDL_SetRenderDrawColor(Setup::renderer, 180, 180, 180, 255);
        SDL_RenderFillRect(Setup::renderer, &scrollbar);
    }   }
    if (Button({595, 700, 76, 35}, {255, 140, 66, 180}, "Back", {600, 700}, 24))
    {
        CurrentLayer = 1;
        UIAction = 0;
    }
}
void UI::ScoreBoard() {
    const int entryHeight = 30;
    int maxVisibleEntries = 10;
    if (Setup::ScoreHistory.size() < 11) {
        maxVisibleEntries = Setup::ScoreHistory.size(); 
    }
    const int totalEntries = Setup::ScoreHistory.size();
    SDL_FRect SettingsOutline = {500, 300, 900, 500};
    SDL_SetRenderDrawColor(Setup::renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(Setup::renderer, &SettingsOutline);
    SDL_SetRenderDrawColor(Setup::renderer, 255, 255, 255, 255);
    SDL_RenderRect(Setup::renderer, &SettingsOutline);
    int firstIndex = Scrolling / entryHeight;
    int lastIndex = std::min(firstIndex + maxVisibleEntries, totalEntries);

    int maxScroll = std::max(0, (totalEntries - maxVisibleEntries) * entryHeight);
    Scrolling = std::clamp(Scrolling, 0, maxScroll);
    for (int i = firstIndex; i < lastIndex; i++) {
        float yOffset = 350.0f - (Scrolling % entryHeight) + ((i - firstIndex) * entryHeight);
        std::string score = std::to_string(i+1) + " ------------- " + std::to_string(Setup::ScoreHistory[i]);
        TextManager::RenderText(score.c_str(), {600, yOffset}, {255, 255, 255, 255}, 24);
    }
    if (totalEntries > maxVisibleEntries) {
        float scrollbarHeight = SettingsOutline.h * ((float)maxVisibleEntries / totalEntries);
        float scrollbarY = SettingsOutline.y + (float(Scrolling) / maxScroll) * (SettingsOutline.h - scrollbarHeight);

        SDL_FRect scrollbar = {
            SettingsOutline.x + SettingsOutline.w - 10,
            scrollbarY,
            6,
            scrollbarHeight
        };

        SDL_SetRenderDrawColor(Setup::renderer, 180, 180, 180, 255);
        SDL_RenderFillRect(Setup::renderer, &scrollbar);
    }
    if (Button({595, 700, 76, 35}, {255, 140, 66, 180}, "Back", {600, 700}, 24))
    {
        CurrentLayer = 1;
        UIAction = 0;
    }
}
void UI::Store() {
    SDL_FRect StoreOutline = {500, 300, 900, 500};
    SDL_SetRenderDrawColor(Setup::renderer, 0, 0, 0, 180);
    SDL_RenderFillRect(Setup::renderer, &StoreOutline);
    SDL_SetRenderDrawColor(Setup::renderer, 255, 255, 255, 255);
    SDL_RenderRect(Setup::renderer, &StoreOutline);

    SDL_FRect BCIcon = {580, 340, 128, 128};
    if (Setup::TargetCoins < 80) {
        TextureManager::DrawTextureNP(40, Setup::renderer, &BCIcon, 0); 
        TextManager::RenderText("Unavailable", {550, 470}, {255, 255, 255, 255}, 16);
    }
    else {
        if (Button({550, 350, 180, 165}, {255, 255, 255, 100}, "Heal +20 Health" , {555, 470}, 16))
        {
            Setup::TargetCoins -= 80;
            Setup::EntityList[0].HP = std::min(Setup::EntityList[0].HP + 0.2f, Player::PlayerUpgrades.TotalHP);

        }
        TextureManager::DrawTextureNP(9, Setup::renderer, &BCIcon, 0); 
    }
    TextManager::RenderText("Price: 80", {550, 350}, {255, 50, 50, 255}, 14);

    BCIcon = {790, 340, 128, 128};
    if (Setup::TargetCoins < 100) {
        TextureManager::DrawTextureNP(40, Setup::renderer, &BCIcon, 0); 
        TextManager::RenderText("Unavailable", {760, 470}, {255, 255, 255, 255}, 16);
    }
    else {
        if (Button({760, 350, 180, 165}, {255, 255, 255, 100}, "Increase Max", {765, 470}, 16))
        {
            Setup::TargetCoins -= 100;
            Player::PlayerUpgrades.TotalHP += 0.2f;
            Setup::EntityList[0].HP = std::min(Setup::EntityList[0].HP + 0.2f, Player::PlayerUpgrades.TotalHP);
        }
        TextManager::RenderText("Health +20", {765, 490}, {255, 255, 255, 255}, 16);
        TextureManager::DrawTextureNP(15, Setup::renderer, &BCIcon, 0); 
    }
    TextManager::RenderText("Price: 100", {760, 350}, {255, 50, 50, 255}, 14);

    BCIcon = {1000, 340, 128, 128};
    if (Setup::TargetCoins < 80) {
        TextureManager::DrawTextureNP(40, Setup::renderer, &BCIcon, 0); 
        TextManager::RenderText("Unavailable", {970, 470}, {255, 255, 255, 255}, 16);
    }
    else {
        if (Button({970, 350, 180, 165}, {255, 255, 255, 100}, "Boost Max", {975, 470}, 16))
        {
            Setup::TargetCoins -= 80;
            Player::PlayerUpgrades.TotalBulletCap += 0.2f;
            Setup::EntityList[0].BC = std::min(Setup::EntityList[0].BC + 0.2f, Player::PlayerUpgrades.TotalBulletCap);
        }
        TextManager::RenderText("Ammo +20", {975, 490}, {255, 255, 255, 255}, 16);
        TextureManager::DrawTextureNP(19, Setup::renderer, &BCIcon, 0); 
    }
    TextManager::RenderText("Price: 80", {970, 350}, {255, 50, 50, 255}, 14);

    BCIcon = {1210, 340, 128, 128};
    if (Setup::TargetCoins < 150) {
        TextureManager::DrawTextureNP(40, Setup::renderer, &BCIcon, 0); 
        TextManager::RenderText("Unavailable", {1185, 470}, {255, 255, 255, 255}, 16);
    }
    else {
        if (Button({1180, 350, 180, 165}, {255, 255, 255, 100}, "Upgrade Bullet", {1185, 470}, 16))
        {
            Setup::TargetCoins -= 150;
            Player::PlayerUpgrades.damagePerBullet += 0.05f;
        }
        TextManager::RenderText("Damage +5", {1185, 490}, {255, 255, 255, 255}, 16);
        TextureManager::DrawTextureNP(17, Setup::renderer, &BCIcon, 0); 
    }
    TextManager::RenderText("Price: 150", {1180, 350}, {255, 50, 50, 255}, 14);

    BCIcon = {580, 544, 128, 128};
    if (Setup::TargetCoins < 200) {
        TextureManager::DrawTextureNP(40, Setup::renderer, &BCIcon, 0); 
        TextManager::RenderText("Unavailable", {550, 674}, {255, 255, 255, 255}, 16);
    }
    else {
        if (Button({550, 554, 180, 165}, {255, 255, 255, 100}, "Multi-Shot:", {555, 660}, 16))
        {
            Player::upgradedPlayer = true;
            Setup::TargetCoins -= 200;
            Player::PlayerUpgrades.extraBulletsPerShot += 1;
        }
        TextManager::RenderText("+1 Bullet", {555, 680}, {255, 255, 255, 255}, 16);
        TextureManager::DrawTextureNP(19, Setup::renderer, &BCIcon, 0); 
        BCIcon = {595, 595, 32, 32};
        TextureManager::DrawTextureNP(9, Setup::renderer, &BCIcon, 0); 
    }
    TextManager::RenderText("Price: 200", {550, 554}, {255, 50, 50, 255}, 14);

    BCIcon = {790, 544, 128, 128};
    if (Setup::TargetCoins < 300 || Player::PlayerUpgrades.firemodes[0]) {
        TextureManager::DrawTextureNP(40, Setup::renderer, &BCIcon, 0); 
        TextManager::RenderText("Unavailable", {760, 674}, {255, 255, 255, 255}, 16);
    }
    else {
        if (Button({760, 554, 180, 165}, {255, 255, 255, 100}, "Unlock Shotgun", {765, 660}, 16))
        {
            Player::upgradedPlayer = true;
            Setup::TargetCoins -= 300;
            Player::PlayerUpgrades.firemodes[0] = true;
        }
        TextManager::RenderText("Spread", {765, 680}, {255, 255, 255, 255}, 16);
        TextureManager::DrawTextureNP(18, Setup::renderer, &BCIcon, 0); 
    }   
    TextManager::RenderText("Price: 300", {760, 554}, {255, 50, 50, 255}, 14);

    BCIcon = {1000, 544, 128, 128};
    if (Setup::TargetCoins < 300 || Player::PlayerUpgrades.firemodes[1]) {
        TextureManager::DrawTextureNP(40, Setup::renderer, &BCIcon, 0); 
        TextManager::RenderText("Unavailable", {970, 674}, {255, 255, 255, 255}, 16);
    }
    else {
        if (Button({970, 554, 180, 165}, {255, 255, 255, 100}, "Unlock Radial Fire", {975, 660}, 16))
        {
            Player::upgradedPlayer = true;
            Setup::TargetCoins -= 300;
            Player::PlayerUpgrades.firemodes[1] = true;
        }
        TextureManager::DrawTextureNP(16, Setup::renderer, &BCIcon, 0); 
    }
    TextManager::RenderText("Price: 300", {970, 554}, {255, 50, 50, 255}, 14);

    BCIcon = {1210, 544, 128, 128};
    if (Setup::TargetCoins < 300 || Player::PlayerUpgrades.firemodes[2]) {
        TextureManager::DrawTextureNP(40, Setup::renderer, &BCIcon, 0); 
        TextManager::RenderText("Unavailable", {1180, 674}, {255, 255, 255, 255}, 16);
    }
    else {
        if (Button({1180, 554, 180, 165}, {255, 255, 255, 100}, "Unlock Parallel", {1185, 660}, 16))
        {
            Player::upgradedPlayer = true;
            Setup::TargetCoins -= 300;
            Player::PlayerUpgrades.firemodes[2] = true;
        }
        TextManager::RenderText("Fire", {1185, 680}, {255, 255, 255, 255}, 16);
        TextureManager::DrawTextureNP(19, Setup::renderer, &BCIcon, 0);
    }
    TextManager::RenderText("Price: 300", {1180, 554}, {255, 50, 50, 255}, 14);
    if (Button({1200, 740, 150, 35}, {255, 140, 66, 180}, "Next Level", {1205, 740}, 24))
    {
        Setup::is_Paused = false;
        UIAction = 0;
        CurrentLayer = 1;
        NextLevelPopUp();
    }
}
void UI::LevelUp() {
    SDL_FRect LevelOutline = {500, 300, 900, 500};
    SDL_SetRenderDrawColor(Setup::renderer, 0, 0, 0, 180);
    SDL_RenderFillRect(Setup::renderer, &LevelOutline);
    SDL_SetRenderDrawColor(Setup::renderer, 255, 255, 255, 255);
    SDL_RenderRect(Setup::renderer, &LevelOutline);
    if (Button({550, 400, 180, 260}, {255, 255, 255, 100}, "Battle-Hardened" , {555, 550}, 18))
    {
        Setup::is_Paused = false;
        UIAction = 0;
        CurrentLayer = 1;
        Player::PlayerUpgrades.ExperienceP = 0;
        Player::PlayerUpgrades.bulletCooldownMultiplier *= 0.9f;
    }
    TextManager::RenderText("Loader", {555, 570}, {255, 255, 255, 255}, 18);
    TextManager::RenderText("Slightly Decrease", {555, 590}, {255, 255, 255, 255}, 16);
    TextManager::RenderText("Bullet Cooldown", {555, 610}, {255, 255, 255, 255}, 16);
    TextManager::RenderText("-10%", {555, 630}, {255, 255, 255, 255}, 16);
    SDL_FRect BCIcon = {580, 420, 128, 128};
    TextureManager::DrawTextureNP(13, Setup::renderer, &BCIcon, 0); 
    if (Button({760, 400, 180, 260}, {255, 255, 255, 100}, "Sharpened Aim", {765, 550}, 18))
    {
        Setup::is_Paused = false;
        UIAction = 0;
        CurrentLayer = 1;
        Player::PlayerUpgrades.ExperienceP = 0;
        Player::PlayerUpgrades.damagePerBullet *= 1.15f;
    }
    TextManager::RenderText("Slightly Increase", {765, 570}, {255, 255, 255, 255}, 16);
    TextManager::RenderText("Bullet Damage", {765, 590}, {255, 255, 255, 255}, 16);
    TextManager::RenderText("+15%", {765, 610}, {255, 255, 255, 255}, 16);
    BCIcon = {790, 420, 128, 128};
    TextureManager::DrawTextureNP(17, Setup::renderer, &BCIcon, 0); 
    if (Button({970, 400, 180, 260}, {255, 255, 255, 100}, "Efficient Repair", {975, 550}, 18))
    {
        Setup::is_Paused = false;
        UIAction = 0;
        CurrentLayer = 1;
        Player::PlayerUpgrades.ExperienceP = 0;
        Player::PlayerUpgrades.PassiveHealing += 0.01f;
    }
    TextManager::RenderText("Protocols", {975, 570}, {255, 255, 255, 255}, 16);
    TextManager::RenderText("Slight Passive", {975, 590}, {255, 255, 255, 255}, 16);
    TextManager::RenderText("HP Regen +1%", {975, 610}, {255, 255, 255, 255}, 16);
    BCIcon = {1000, 420, 128, 128};
    TextureManager::DrawTextureNP(23, Setup::renderer, &BCIcon, 0); 
    if (Button({1180, 400, 180, 260}, {255, 255, 255, 100}, "Thruster", {1185, 550}, 18))
    {
        Setup::is_Paused = false;
        UIAction = 0;
        CurrentLayer = 1;
        Player::PlayerUpgrades.ExperienceP = 0;
        Player::PlayerUpgrades.MovementSpeed *= 1.1f;
    }
    TextManager::RenderText("Synchronization", {1185, 570}, {255, 255, 255, 255}, 18);
    TextManager::RenderText("Slightly Increase", {1185, 590}, {255, 255, 255, 255}, 16);
    TextManager::RenderText("Movement Speed", {1185, 610}, {255, 255, 255, 255}, 16);
    TextManager::RenderText("+10%", {1185, 630}, {255, 255, 255, 255}, 16);
    BCIcon = {1210, 420, 128, 128};
    TextureManager::DrawTextureNP(20, Setup::renderer, &BCIcon, 0); 

}
void UI::Escape()
{
    SDL_FRect SettingsOutline = {500, 300, 900, 500};
    SDL_SetRenderDrawColor(Setup::renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(Setup::renderer, &SettingsOutline);
    SDL_SetRenderDrawColor(Setup::renderer, 255, 255, 255, 255);
    SDL_RenderRect(Setup::renderer, &SettingsOutline);
    if (video_settings || audio_settings || misc_settings)
    {
        CurrentLayer = 3;
    }
    if (!video_settings) video_settings = Button({595, 400, 85, 35}, {255, 140, 66, 180}, "Video", {600, 400}, 24);
    if (!audio_settings) audio_settings = Button({595, 500, 85, 35}, {255, 140, 66, 180}, "Audio", {600, 500}, 24);
    if (!misc_settings) misc_settings = Button({595, 600, 73, 35}, {255, 140, 66, 180}, "Misc", {600, 600}, 24);
    if (video_settings)
    {
        SDL_SetRenderDrawColor(Setup::renderer, 0, 0, 0, 255);
        SDL_FRect fillRect = {500, 300, 900, 500}; //clears the settings area
        SDL_RenderFillRect(Setup::renderer, &fillRect);

        SDL_SetRenderDrawColor(Setup::renderer, 255, 255, 255, 255);
        SDL_RenderRect(Setup::renderer, &SettingsOutline);
        TextManager::RenderText("Screen Mode: ", {700, 400}, {255, 255, 255, 255}, 24);
        if (Button({1000, 400, 150, 50}, {200, 200, 200, 200}, "FullScreen", {1000, 400}, 24))
        {
            SDL_SetWindowFullscreen(Setup::window, true);
        }
        if (Button({1170, 400, 150, 50}, {200, 200, 200, 200}, "Windowed", {1170, 400}, 24))
        {
            SDL_SetWindowFullscreen(Setup::window, false);
        }
        TextManager::RenderText("Vsync: ", {700, 450}, {255, 255, 255, 255}, 24);
        TextManager::RenderText("Resolution: ", {700, 500}, {255, 255, 255, 255}, 24);
        static int Dropdownchoice;
        if (Setup::WindowWidth == 1920)
        {
            Dropdownchoice = 1;
        }
        Dropdownchoice = Dropdown(2, Dropdownchoice, {1000, 500, 200, 50}, {"--------", "1920x1080", "1280x720"});
        if (Dropdownchoice == 1 && Setup::WindowWidth != 1920)
        {
            Setup::WindowWidth = 1920;
            Setup::WindowHeight = 1080;
            SDL_SetWindowSize(Setup::window, Setup::WindowWidth, Setup::WindowHeight);
            SDL_RestoreWindow(Setup::window);
            SDL_ShowWindow(Setup::window);
            SDL_GetWindowSize(Setup::window, &Setup::WindowWidth, &Setup::WindowHeight);
        }
        else if (Dropdownchoice == 2 && Setup::WindowWidth != 1280)
        {
            Setup::WindowWidth = 1280;
            Setup::WindowHeight = 720;
            SDL_SetWindowSize(Setup::window, Setup::WindowWidth, Setup::WindowHeight);
            SDL_RestoreWindow(Setup::window);
            SDL_ShowWindow(Setup::window);
            SDL_GetWindowSize(Setup::window, &Setup::WindowWidth, &Setup::WindowHeight);
        }

    }
    if (audio_settings)
    {
        SDL_SetRenderDrawColor(Setup::renderer, 0, 0, 0, 255);
        SDL_FRect fillRect = {500, 300, 900, 500};
        SDL_RenderFillRect(Setup::renderer, &fillRect);

        SDL_SetRenderDrawColor(Setup::renderer, 255, 255, 255, 255);
        SDL_RenderRect(Setup::renderer, &SettingsOutline);
        TextManager::RenderText("Master Volume: ", {700, 400}, {255, 255, 255, 255}, 24);
        Sound::VolumeAdjustmentChannel[0] = Slider({1000, 400, 200, 30}, Sound::VolumeAdjustmentChannel[0]);
        Mix_MasterVolume(Sound::VolumeAdjustmentChannel[0]);
        TextManager::RenderText("Music Volume: ", {700, 450}, {255, 255, 255, 255}, 24);
        Mix_VolumeMusic(Slider({1000, 450, 200, 30}, Mix_VolumeMusic(-1)));
        TextManager::RenderText("Menu Volume: ", {700, 500}, {255, 255, 255, 255}, 24);
        Sound::VolumeAdjustmentChannel[1] = Slider({1000, 500, 200, 30}, Sound::VolumeAdjustmentChannel[1]);
        TextManager::RenderText("Player Volume: ", {700, 550}, {255, 255, 255, 255}, 24);
        Sound::VolumeAdjustmentChannel[2] = Slider({1000, 550, 200, 30}, Sound::VolumeAdjustmentChannel[2]);
        TextManager::RenderText("Enemy Volume: ", {700, 600}, {255, 255, 255, 255}, 24);
        Sound::VolumeAdjustmentChannel[3] = Slider({1000, 600, 200, 30}, Sound::VolumeAdjustmentChannel[3]);
    }
    if (misc_settings)
    {
        SDL_SetRenderDrawColor(Setup::renderer, 0, 0, 0, 255);
        SDL_FRect fillRect = {500, 300, 900, 500};
        SDL_RenderFillRect(Setup::renderer, &fillRect);

        SDL_SetRenderDrawColor(Setup::renderer, 255, 255, 255, 255);
        SDL_RenderRect(Setup::renderer, &SettingsOutline);
        TextManager::RenderText("-----", {700, 400}, {255, 255, 255, 255}, 24);
        TextManager::RenderText("-----", {700, 450}, {255, 255, 255, 255}, 24);
        TextManager::RenderText("-----", {700, 450}, {255, 255, 255, 255}, 24);
    }
    if (Button({595, 700, 76, 35}, {255, 140, 66, 180}, "Back", {600, 700}, 24))
    {
        if (video_settings || audio_settings || misc_settings)
        {
            CurrentLayer = 2;
            video_settings = false;
            audio_settings = false;
            misc_settings = false;
        }
        else
        {
            CurrentLayer = 1;
            UIAction = 0;
        }
    }
    if (CurrentMenu == "Swarm_Mode")
    {
        if (Button({1145, 700, 150, 35}, {255, 140, 66, 180}, "Main Menu", {1150, 700}, 24))
        {
            CurrentMenu = "Main_Menu";
            UIAction = 0;
            CurrentLayer = 1;
        }
    }
}

bool LayerValidation(const char* ButtonName)
{
    auto ButtonRange = ButtonLayout.equal_range(UI::CurrentLayer);
    for (auto itr = ButtonRange.first; itr != ButtonRange.second; ++itr)
    {
        if (std::strcmp(itr->second, ButtonName) == 0)
        {
            return true;
        }
    }
    return false;
}



bool UI::Button(SDL_FRect ButtonDimensions, SDL_Color ButtonColor, const char* ButtonName, Vector TextDimensions, int TextSize)
{
    ButtonDimensions.y += 3.5;
    ButtonDimensions.x += 3.5;
    SDL_SetRenderDrawColor(Setup::renderer, 50, 50, 50, ButtonColor.a);
    SDL_RenderFillRect(Setup::renderer, &ButtonDimensions);

    ButtonDimensions.y -= 3.5;
    ButtonDimensions.x -= 3.5;
    SDL_SetRenderDrawColor(Setup::renderer, ButtonColor.r, ButtonColor.g, ButtonColor.b, ButtonColor.a);
    SDL_RenderFillRect(Setup::renderer, &ButtonDimensions);

    TextManager::RenderText(ButtonName, {TextDimensions.x, TextDimensions.y}, {255, 255, 255, 255}, TextSize);
    SDL_FPoint LeftClickMousePos = {Setup::mouseCoordin.x, Setup::mouseCoordin.y};
    if (LayerValidation(ButtonName))
    {
        if (SDL_PointInRectFloat(&LeftClickMousePos, &ButtonDimensions))
        {
            SDL_SetRenderDrawColor(Setup::renderer, ButtonColor.r, ButtonColor.g, ButtonColor.b, ButtonColor.a);
            SDL_RenderFillRect(Setup::renderer, &ButtonDimensions);

            SDL_SetRenderDrawColor(Setup::renderer, ButtonColor.r, ButtonColor.g, ButtonColor.b, ButtonColor.a + 5);
            SDL_RenderRect(Setup::renderer, &ButtonDimensions);
            if (Player::LeftMouse && buttonCooldown <= 0)
            {
                buttonCooldown = 60;
                Sound::PlaySpecificChannelSound(4, 0);
                return true;
            }
        }
    }
    return false;
}

bool UI::Button(SDL_FRect ButtonDimensions, SDL_Color ButtonColor, const char* ButtonName, Vector TextDimensions, int TextSize, SDL_Color TextColor)
{
    ButtonDimensions.y += 3.5;
    ButtonDimensions.x += 3.5;
    SDL_SetRenderDrawColor(Setup::renderer, 50, 50, 50, ButtonColor.a);
    SDL_RenderFillRect(Setup::renderer, &ButtonDimensions);

    ButtonDimensions.y -= 3.5;
    ButtonDimensions.x -= 3.5;
    SDL_SetRenderDrawColor(Setup::renderer, ButtonColor.r, ButtonColor.g, ButtonColor.b, ButtonColor.a);
    SDL_RenderFillRect(Setup::renderer, &ButtonDimensions);

    TextManager::RenderText(ButtonName, {TextDimensions.x, TextDimensions.y}, TextColor, TextSize);
    SDL_FPoint LeftClickMousePos = {Setup::mouseCoordin.x, Setup::mouseCoordin.y};
    if (LayerValidation(ButtonName))
    {
        if (SDL_PointInRectFloat(&LeftClickMousePos, &ButtonDimensions))
        {
            SDL_SetRenderDrawColor(Setup::renderer, ButtonColor.r, ButtonColor.g, ButtonColor.b, ButtonColor.a);
            SDL_RenderFillRect(Setup::renderer, &ButtonDimensions);

            SDL_SetRenderDrawColor(Setup::renderer, ButtonColor.r, ButtonColor.g, ButtonColor.b, ButtonColor.a + 5);
            SDL_RenderRect(Setup::renderer, &ButtonDimensions);
            if (Player::LeftMouse && buttonCooldown <= 0)
            {
                buttonCooldown = 60;
                Sound::PlaySpecificChannelSound(4, 0);
                return true;
            }
        }
    }
    return false;
}

int UI::Dropdown(int choice_count, int current_choice, SDL_FRect DropdownDimensions, std::vector<const char*> DropdownOptions)
{
    static bool opendropd;
    // ButtonLayout.insert(std::pair<int, const char*>(3, DropdownOptions[current_choice]));
    SDL_FRect ButtonDim = DropdownDimensions;
    bool insideopendropd;
    if (!opendropd)
    {
        insideopendropd = Button(ButtonDim, {200, 200, 200, 200}, DropdownOptions[current_choice - 1], {ButtonDim.x, ButtonDim.y}, 22);
    }
    if (insideopendropd)
    {
        opendropd = true;

    }
    if (opendropd)
    {
        SDL_SetRenderDrawColor(Setup::renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(Setup::renderer, &ButtonDim);
        for (int i = 0; i <= choice_count; i++)
        {
            ButtonDim.y = DropdownDimensions.y + (DropdownDimensions.h + 5) * i;

            if (Button(ButtonDim, {200, 200, 200, 200}, DropdownOptions[i], {ButtonDim.x, ButtonDim.y}, 22))
            {
                current_choice = i;
                opendropd = false;
            }
        }
    }
    return current_choice;
}


int UI::Slider(SDL_FRect dest, int volume)
{
    SDL_SetRenderDrawColor(Setup::renderer, 255, 255, 255, 235);
    SDL_RenderRect(Setup::renderer, &dest);
    SDL_SetRenderDrawColor(Setup::renderer, 200, 200, 200, 235);
    SDL_FRect destFill = {dest.x, dest.y, dest.w * (static_cast<float>(volume) / 128.0f), dest.h};
    SDL_RenderFillRect(Setup::renderer, &destFill);
    SDL_SetRenderDrawColor(Setup::renderer, 255, 255, 255, 235);
    SDL_FRect sliderRect = {
        dest.x + dest.w * (static_cast<float>(volume) / 128.0f), dest.y - 5, dest.w / 20, dest.h + 15
    };
    SDL_RenderFillRect(Setup::renderer, &sliderRect);
    SDL_FPoint LeftMousePos = {Setup::mouseCoordin.x, Setup::mouseCoordin.y};
    if (SDL_PointInRectFloat(&LeftMousePos, &dest))
    {
        if (Player::LeftMouse)
        {
            int calc = (dest.x - LeftMousePos.x) / dest.w * 128;
            if (calc < 0)
            {
                calc = calc * -1;
            }
            calc = std::clamp(calc, 0,  128);
            volume = calc;
        }
    }
    return volume;
}

bool UI::Main_Menu()
{
    if (CurrentMenu == "Main_Menu")
    {
        Player::playerInput();
        Setup::is_Paused = true;
        RenderUI();
        return true;
    }
    else
    {
        return false;
    }
}

void UI::Cleanup()
{
    if (UITexture)
    {
        SDL_DestroyTexture(UITexture);
        UITexture = nullptr;
    }
}
