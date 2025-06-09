//
// Created by romak on 16.01.2025.
//

#include "Setup.h"

#include <iostream>
#include <ostream>
#include <fstream>
#include <cstdlib>
#include <filesystem>

#include "Bullet.h"
#include "Enemy.h"
#include "Map.h"
#include "Player.h"
#include "Sound.h"
#include "TextManager.h"
#include "TextureManager.h"
#include "UI.h"
#include "Loot.h"

Setup Setup;
int Setup::Score = 0;
int Setup::Coins = 0;
int Setup::TargetCoins = 0;
int Setup::TargetScore = 0;
bool Setup::Restart = false;
bool Setup::Vsync = false;
bool Setup::loopRunning = true;
bool Setup::is_Paused = true;
float Setup::Max_FPS = 144.0f;
SDL_Event Setup::event;
SDL_Window* Setup::window = nullptr;
int Setup::WindowWidth = 1920;
int Setup::WindowHeight = 1080;
SDL_Renderer* Setup::renderer = nullptr;
Vector Setup::mouseCoordin = {};
float Setup::speed = 5.;
float Setup::Difficulty = 1.;
std::vector<EntityStats> Setup::EntityList;

std::string Setup::basePath;

int Setup::CurrentLevel;
std::vector<int> Setup::ScoreHistory;

void Setup::initialization()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
    }
    if (!SDL_Init(SDL_INIT_EVENTS))
    {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
    }
    if (!SDL_Init(SDL_INIT_AUDIO))
    {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
    }
    if (!Mix_Init(MIX_INIT_WAVPACK))
    {
        std::cout << "Mix_Init Error: " << SDL_GetError() << std::endl;
    }
    if (!TTF_Init())
    {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << SDL_GetError() << std::endl;
    }
    window = SDL_CreateWindow("Game2D", WindowWidth, WindowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
    if (window == nullptr)
    {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        loopRunning = false;
        return;
    }
    const char* cwd = SDL_GetBasePath();
    basePath = cwd;
    basePath.erase(basePath.length() - 6);
    std::replace(basePath.begin(), basePath.end(), '\\', '/');
    std::cout << "Base path: " << basePath << std::endl;
    SDL_Surface* icon = IMG_Load("A:/C++/Game2D/assets/icon.png");
    SDL_SetWindowIcon(window, icon);
    SDL_DestroySurface(icon);
    renderer = SDL_CreateRenderer(window, "opengl");
    SDL_SetRenderVSync(renderer, Vsync);

    // Debug: Reinitialize the texture to ensure it loads
    if (!Bullet::textureBullet)
    {
        Bullet::textureBullet = TextureManager::LoadTexture("laser.png");
    }
    UI::Init();
    Sound::Init();
    Sound::PlayMusic("space-158081.mp3", 0);
    //Loading animations
IMG_Animation* explosion = TextureManager::LoadAnimation("explosion.gif");
IMG_Animation* deathAnimationSuicide = TextureManager::LoadAnimation("torpedodest.gif");
IMG_Animation* deathAnimationBomber = TextureManager::LoadAnimation("bomberdest.gif");
IMG_Animation* deathAnimationDreadnought = TextureManager::LoadAnimation("dreadnoughtdest.gif");
IMG_Animation* beam = TextureManager::LoadAnimation("beam.gif");
IMG_Animation* middlebeam = TextureManager::LoadAnimation("middlebeam.gif");
IMG_Animation* dreadnoughtweapon = TextureManager::LoadAnimation("dreadnoughtlaser.gif");
IMG_Animation* enginesPlayer = TextureManager::LoadAnimation("enginesPlayer.gif");
IMG_Animation* enginesBomber = TextureManager::LoadAnimation("enginesBomber.gif");
IMG_Animation* enginesTorpedo = TextureManager::LoadAnimation("enginesTorpedo.gif");
IMG_Animation* enginesDreadnought = TextureManager::LoadAnimation("enginesDreadnought.gif");
    TextureManager::ImgAnimationsVec.emplace_back(deathAnimationSuicide);
    TextureManager::ImgAnimationsVec.emplace_back(explosion);
    TextureManager::ImgAnimationsVec.emplace_back(deathAnimationBomber);
    TextureManager::ImgAnimationsVec.emplace_back(deathAnimationDreadnought);
    TextureManager::ImgAnimationsVec.emplace_back(beam);
    TextureManager::ImgAnimationsVec.emplace_back(middlebeam);
    TextureManager::ImgAnimationsVec.emplace_back(dreadnoughtweapon);
    TextureManager::ImgAnimationsVec.emplace_back(enginesPlayer);
    TextureManager::ImgAnimationsVec.emplace_back(enginesBomber);
    TextureManager::ImgAnimationsVec.emplace_back(enginesTorpedo);
    TextureManager::ImgAnimationsVec.emplace_back(enginesDreadnought);
    //Loading ship textures
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("trimmed_playership.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("trimmed_bomber.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("trimmed_torpedo.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("tdreadnought.png"));
    //Loading icons
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/Skillicon_01.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/Skillicon_02.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/Skillicon_05.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/Skillicon_07.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/Skillicon_11.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/Skillicon_13.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/Skillicon_14.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/Skillicon_16.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/Skillicon_17.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/Skillicon_19.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/Skillicon_21.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/Skillicon_22.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/Skillicon_23.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/Skillicon_25.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/Skillicon_30.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/Skillicon_32.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/Skillicon_33.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/Skillicon_36.png")); 
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/Skillicon_27.png")); 
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/Skillicon_34.png")); //element -> 23
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/keyboard_e.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/keyboard_e_outline.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/keyboard_q.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/keyboard_q_outline.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/keyboard_escape.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/keyboard_escape_outline.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/keyboard_w.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/keyboard_w_outline.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/keyboard_s.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/keyboard_s_outline.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/keyboard_a.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/keyboard_a_outline.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/keyboard_d.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/keyboard_d_outline.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/mouse_move.png")); //element -> 38
    //TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/keyboard_arrows_all.png"));
    //TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/keyboard_arrows_all.png"));
    //TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/keyboard_arrows_down.png"));
    //TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/keyboard_arrows_up.png"));
    //TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/keyboard_arrows_right.png"));
    //TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/keyboard_arrows_left.png"));
    //Loading maps
    if (Map::MapTextureNames.empty()) {
        Map::MapTextureNames.emplace_back("Space_Background.png");
        Map::MapTextureNames.emplace_back("Space_Background1.png");
        Map::MapTextureNames.emplace_back("Space_Background2.png");
        Map::MapTextureNames.emplace_back("Space_Background3.png");
        Map::MapTextureNames.emplace_back("Space_Background4.png");
        Map::MapTextureNames.emplace_back("Space_Background5.png");
    }
    Mix_VolumeMusic(12);
    Map::MapGeneration();
    CurrentLevel = 1;
    Map::NewMap(CurrentLevel);
    ScoreHistory.emplace_back(300);
    ScoreHistory.emplace_back(200);
    ScoreHistory.emplace_back(100);
}
void Setup::gameLoop()
{
    unsigned int a = SDL_GetTicks();
    unsigned int b = SDL_GetTicks();
    double delta = 0;
    while (loopRunning)
    {
        if (!Vsync)
        {
            a = SDL_GetTicks();
            delta = a - b;
        }
        if (UI::Main_Menu() != true && delta >= 1000/Max_FPS)
        {
            b = a;
            update();
            render();
        }
    }
}
void Setup::restart()
{
    if (Restart)
    {
        EntityList.clear();
        Bullet::BulletList.clear();
        Bullet::ScheduledBulletList.clear();
        TextureManager::animationsVec.clear();
        Loot::CoinVector.clear();
        Player player;
        EntityList.emplace_back(player.HP, player.BC, 0, player.x, player.y, player.TextureID, player.srcR, player.dstR, player.rotation, player.velocity, player.shooting_delay);
        Restart = false;
        is_Paused = false;
        Mix_HaltChannel(-1);
        Enemy::RestartWaves();
        Score += (0.2 * Player::coin_pickup_played) + (10 * CurrentLevel);
        auto it = std::lower_bound(ScoreHistory.begin(), ScoreHistory.end(), TargetScore, [](int a, int b) { return a > b; });
        ScoreHistory.insert(it, TargetScore);
        Score = 0;
        TargetScore = 0;
        Coins = 0;
        TargetCoins = 0;
        UI::CurrentLayer = 1;
        Sound::PlaySound(11);
        Player::PlayerUpgrades = PlayerUpgradeStats{};
        Map::MapGeneration();
        CurrentLevel = 1;
        Map::NewMap(CurrentLevel);

    }
}
void Setup::nextLevel()
{
    Bullet::BulletList.clear();
    Bullet::ScheduledBulletList.clear();
    TextureManager::animationsVec.clear();
    Player::hitPlayerLevel = false;
    is_Paused = true;
    UI::CurrentLayer = 4;
    UI::UIAction = 4;
    UI::Store();
    Map::MapGeneration();
    CurrentLevel++;
    if (CurrentLevel > 6) {
        Restart = true;
        Sound::PlaySound(11);
        restart();
    }
    Map::NewMap(CurrentLevel);
    Mix_HaltChannel(-1);
    Enemy::RestartWaves();
    Sound::PlaySound(11);
    UI::CurrentLayer = 1;
}
void Setup::update()
{
    Player::playerInput();
    Sound::PAndUNP_SoundEffects(true);
    if (Restart)
    {
        restart();
    }
    if (!is_Paused)
    {
        Sound::PAndUNP_SoundEffects(false);
        Player::AchievementLogging();
        Sound::VolumeAdjustment();
        /*
        is_Paused = true;
        UI::CurrentLayer = 4;
        UI::UIAction = 4;
        UI::Store();
        */
        Enemy::SwarmingScenario();
        for (auto& Entity : Setup::EntityList)
        {
            if (Entity.type == 0) {
                Entity.x += Entity.velocity.x * (speed * Player::PlayerUpgrades.MovementSpeed);
                Entity.y += Entity.velocity.y * (speed * Player::PlayerUpgrades.MovementSpeed);
                Entity.dest.x = Entity.x;
                Entity.dest.y = Entity.y;
            }
            else {
                Entity.x += Entity.velocity.x * speed;
                Entity.y += Entity.velocity.y * speed;
                Entity.dest.x = Entity.x;
                Entity.dest.y = Entity.y;
            }
            if (Entity.effectTimer > 0)
            {
                Entity.effectTimer--;
            }
        }
        Loot::UpdateCoins();
        TextureManager::AnimationCleaning(); 
        Bullet::updateBullets();
        Enemy::EnemyAI();
        Map::MapUpdate();
    }
}
struct StoringEffectTimerEntities {
    int type;
    SDL_FRect dest;
};
std::vector<StoringEffectTimerEntities> EntityEffectVec;
void Setup::render()
{
    SDL_RenderClear(renderer);
    Map::MapRender();

    for (auto &Entity : EntityList)
    {
        if (Entity.TextureID >= 0 && Entity.TextureID < TextureManager::TextureVec.size()) {
            if (Entity.effectTimer > 0) {
                SDL_SetTextureColorMod(TextureManager::TextureVec[Entity.TextureID], 255, 100, 100);
                EntityEffectVec.emplace_back(Entity.type, Entity.dest);
            } else {
                SDL_SetTextureColorMod(TextureManager::TextureVec[Entity.TextureID], 255, 255, 255);
            }
            TextureManager::DrawTextureNP(Entity.TextureID, renderer, &Entity.dest, Entity.rotation);
        } else {
            std::cerr << "Invalid TextureID: " << Entity.TextureID << std::endl;
        }
    }
    for (auto& Bullet : Bullet::BulletList)
    {
        TextureManager::DrawTexture(Bullet::textureBullet, renderer, Bullet.srcR, Bullet.dstR, Bullet.rotation);
    }
    for (auto& animation : TextureManager::animationsVec)
    {
        bool colorMod = false;
        if (!EntityEffectVec.empty()) {
            for (auto& Entity : EntityEffectVec) {
            //for (auto& Entity : EntityList) {
                if (animation.AnimationNumber == 6 && Entity.type == 3) {
                    if (SDL_HasRectIntersectionFloat(&Entity.dest, &animation.destRect)) {
                        colorMod = true;
                        }
                }
                /*if (animation.AnimationNumber == 7 + Entity.type)
                    if (SDL_HasRectIntersectionFloat(&Entity.dest, &animation.destRect)) {
                        animation.destRect = Entity.dest;
                }*/
            }
        }
        TextureManager::DrawAnimation(animation, colorMod);
    }
    for (auto& coin : Loot::CoinVector)
    {
        SDL_SetRenderDrawColor(Setup::renderer, 255, 255, 0, 255);
        SDL_RenderFillRect(Setup::renderer, &coin.dst);
        SDL_SetRenderDrawColor(Setup::renderer, 0, 0, 0, 255);
        SDL_RenderRect(Setup::renderer, &coin.dst);
    }
    ScoreChange();
    CoinsChange();
    UI::RenderUI();
    SDL_RenderPresent(renderer);
    EntityEffectVec.clear();
}

void Setup::quit()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Log("Cleaning up...");
    Sound::Cleanup();
    SDL_Quit();
    Bullet::BulletList.clear();
    EntityList.clear();
}
int scoreUpdateDelay = 0;
void Setup::ScoreChange()
{
    if (Score < TargetScore)
    {
        std::string shownewscore = "+" + std::to_string(TargetScore - Score);
        TextManager::RenderText(shownewscore.c_str(), {800, 60}, {255, 255, 255, 255}, 24);
    }
    if (scoreUpdateDelay > 0)
    {
        scoreUpdateDelay--;
        return;
    }

    int change = static_cast<int>((TargetScore - Score) * 0.2f);
    if (change == 0) Score = TargetScore;
    else Score += change;

    scoreUpdateDelay = 7;
}
int coinsUpdateDelay = 0;
void Setup::CoinsChange()
{
    if (Coins < TargetCoins)
    {
        std::string shownewcoins = "+" + std::to_string(TargetCoins - Coins);
        TextManager::RenderText(shownewcoins.c_str(), {1200, 60}, {255, 255, 255, 255}, 24);
    }
    if (coinsUpdateDelay > 0)
    {
        coinsUpdateDelay--;
        return;
    }

    int change = static_cast<int>((TargetCoins - Coins) * 0.2f);
    if (change == 0) Coins = TargetCoins;
    else Coins += change;

    coinsUpdateDelay = 7;
}
