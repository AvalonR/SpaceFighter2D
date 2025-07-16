//
// Created by romak on 16.01.2025.
//

#include "Setup.h"

#include <cstdint>
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

std::mt19937 Setup::rng{std::random_device{}()};
std::unordered_set<uint32_t> Setup::usedIDs;

uint32_t Setup::generateID(int TypeID) {
    if (TypeID < 0 || TypeID > 15) {
        throw std::invalid_argument("Type ID must be between 0 and 15");
    }

    std::uniform_int_distribution<uint32_t> dist{0, (1u << 28) - 1}; // 28 bits for randomness
    uint32_t id;
    do {
        uint32_t base = dist(rng);             // 28-bit random part
        id = (base << 4) | (TypeID & 0xF);     // Embed TypeID in last 4 bits
    } while (usedIDs.count(id));
    
    usedIDs.insert(id);
    return id;
}

void Setup::RandomizeMusic() {
    int music = Enemy::Randomizer(1, 4);
    if (music == 1)
    {
        Sound::PlayMusic("space-158081.mp3", 0);
    }
    if (music == 2)
    {
        Sound::PlayMusic("Aylex-AI.mp3", 0);
    }
    if (music == 3)
    {
        Sound::PlayMusic("Aylex-FF.mp3", 0);
    }
    if (music == 4)
    {
        Sound::PlayMusic("ocean-wave-ambient.mp3", 0);
    }
}
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
    if (!Mix_Init(MIX_INIT_MP3))
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
    std::string assetspath = basePath + "/assets/icon.png";
    SDL_Surface* icon = IMG_Load(assetspath.c_str());
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
    RandomizeMusic();
    //Loading animations
    IMG_Animation* explosion = TextureManager::LoadAnimation("explosion.gif");
    IMG_Animation* deathAnimationSuicide = TextureManager::LoadAnimation("torpedodest.gif");
    IMG_Animation* deathAnimationBomber = TextureManager::LoadAnimation("bomberdest.gif");
    IMG_Animation* deathAnimationDreadnought = TextureManager::LoadAnimation("dreadnoughtdest.gif");
    IMG_Animation* fullbeam = TextureManager::LoadAnimation("fullbeam.gif");
    IMG_Animation* dreadnoughtweapon = TextureManager::LoadAnimation("dreadnoughtlaser.gif");
    IMG_Animation* deathAnimationBattlecruiser = TextureManager::LoadAnimation("NairanBattlecruiserDestruction.gif");
    IMG_Animation* battlecruiserweapon = TextureManager::LoadAnimation("NairanBattlecruiserWeapons.gif");
    IMG_Animation* enginesPlayer = TextureManager::LoadAnimation("enginesPlayer.gif");
    IMG_Animation* enginesBomber = TextureManager::LoadAnimation("enginesBomber.gif");
    IMG_Animation* enginesTorpedo = TextureManager::LoadAnimation("enginesTorpedo.gif");
    IMG_Animation* enginesDreadnought = TextureManager::LoadAnimation("enginesDreadnought.gif");
    IMG_Animation* enginesBattlecruiser = TextureManager::LoadAnimation("enginesBattlecruiser.gif");
    TextureManager::ImgAnimationsVec.emplace_back(deathAnimationSuicide);
    TextureManager::ImgAnimationsVec.emplace_back(explosion);
    TextureManager::ImgAnimationsVec.emplace_back(deathAnimationBomber);
    TextureManager::ImgAnimationsVec.emplace_back(deathAnimationDreadnought);
    TextureManager::ImgAnimationsVec.emplace_back(fullbeam);
    TextureManager::ImgAnimationsVec.emplace_back(dreadnoughtweapon);
    TextureManager::ImgAnimationsVec.emplace_back(deathAnimationBattlecruiser);
    TextureManager::ImgAnimationsVec.emplace_back(battlecruiserweapon);
    TextureManager::ImgAnimationsVec.emplace_back(enginesPlayer);
    TextureManager::ImgAnimationsVec.emplace_back(enginesBomber);
    TextureManager::ImgAnimationsVec.emplace_back(enginesTorpedo);
    TextureManager::ImgAnimationsVec.emplace_back(enginesDreadnought);
    TextureManager::ImgAnimationsVec.emplace_back(enginesBattlecruiser);
    //Loading ship textures
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("trimmed_playership.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("trimmed_bomber.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("trimmed_torpedo.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("tdreadnought.png"));
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("tBattlecruiser.png"));
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
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/Skillicon_34.png")); //element -> 24
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
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/mouse_move.png")); //element -> 39
    TextureManager::TextureVec.emplace_back(TextureManager::LoadTexture("icons/Skillicon_06.png")); //element -> 40
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
        usedIDs.clear();
        Bullet::BulletList.clear();
        Bullet::ScheduledBulletList.clear();
        TextureManager::animationsVec.clear();
        Loot::ParticleVector.clear();
        Enemy::enemyRotations.clear();
        Loot::CoinVector.clear();
        Player player;
        EntityList.emplace_back(player.HP, player.BC, 0, player.x, player.y, player.TextureID, player.srcR, player.dstR, player.rotation, player.velocity, player.shooting_delay, 0, generateID(0));
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
        UI::DifficultyChosen = false;
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
    Loot::ParticleVector.clear();
    Enemy::enemyRotations.clear();
    Player::hitPlayerLevel = false;
    is_Paused = true;
    UI::CurrentLayer = 4;
    UI::UIAction = 4;
    UI::Store();
    Map::MapGeneration();
    CurrentLevel++;
    if (CurrentLevel > 16) {
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
        // is_Paused = true;
        // UI::CurrentLayer = 4;
        // UI::UIAction = 4;
        // UI::Store();
        Enemy::SwarmingScenario();
        for (auto& Entity : Setup::EntityList)
        {
            if (Entity.type == 0) {
                Entity.x += Entity.velocity.x * (speed * Player::PlayerUpgrades.MovementSpeed);
                Entity.y += Entity.velocity.y * (speed * Player::PlayerUpgrades.MovementSpeed);

                Entity.x = std::clamp(Entity.x, 0.0f, WindowWidth - Entity.dest.w);
                Entity.y = std::clamp(Entity.y, 0.0f, WindowHeight - Entity.dest.h);

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
            if (Entity.velocity.x != 0.0f || Entity.velocity.y != 0.0f) {
                bool found = false;

                SDL_FRect a = Entity.dest;
                float centerX = a.x + a.w / 2.0f;
                float centerY = a.y + a.h / 2.0f;
                if (Entity.type == 0) {
                    a.h = a.h * 1.6;
                    a.w = a.w * 1.6;
                }
                if (Entity.type == 1) {
                    a.h = a.h * 2.0;
                    a.w = a.w * 2.0;
                }
                if (Entity.type == 2) {
                    a.w = a.w * 1.1;
                    a.h = a.h * 1.8;
                }
                if (Entity.type == 3) {
                    a.h = a.h * 1.5;
                    a.w = a.w * 1.9;
                }
                if (Entity.type == 4) {
                    a.h = a.h * 1.9;
                    a.w = a.w * 2.1;
                }
                a.x = centerX - a.w / 2.0f;
                a.y = centerY - a.h / 2.0f;
                for (auto& animation : TextureManager::animationsVec) {
                    if (animation.AnimationNumber == 8 + Entity.type && animation.EUID == Entity.UID) {
                        animation.destRect = a;
                        animation.angle = Entity.rotation;
                        found = true;
                        break; 
                    }
                }

                if (!found) {
                    TextureManager::animationsVec.emplace_back( 0, 10, 300, Entity.rotation, 8 + Entity.type, a, Entity.UID);
                }
            } else {
                TextureManager::animationsVec.erase(
                    std::remove_if(TextureManager::animationsVec.begin(), TextureManager::animationsVec.end(),
                        [&](const AnimationVector& anim) {
                            return anim.AnimationNumber == 8 + Entity.type && anim.EUID == Entity.UID;
                        }),
                    TextureManager::animationsVec.end()
                );
            }
        }
        Loot::UpdateCoins();
        Loot::UpdateParticles();
        TextureManager::AnimationCleaning(); 
        Enemy::EnemyAI();
        Bullet::updateBullets();
        Map::MapUpdate();
    }
}
void Setup::render()
{
    SDL_RenderClear(renderer);
    Map::MapRender();
    for (auto& animation : TextureManager::animationsVec)
    {
        if (animation.AnimationNumber >= 8) {
            TextureManager::DrawAnimation(animation, false);
        }
    }
    for (auto &Entity : EntityList)
    {
        if (Entity.TextureID >= 0 && Entity.TextureID < TextureManager::TextureVec.size()) {
            if (Entity.effectTimer > 0) {
                SDL_SetTextureColorMod(TextureManager::TextureVec[Entity.TextureID], 255, 100, 100);
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
        if (animation.AnimationNumber >= 8) {
            continue;
        }
        bool colorMod = false;
        for (auto& Entity : EntityList) {
            if ((animation.AnimationNumber == 5 && Entity.type == 3 && Entity.effectTimer > 0) || (animation.AnimationNumber == 7 && Entity.type == 4 && Entity.effectTimer > 0)) {
                if (Entity.UID == animation.EUID) {
                    colorMod = true;
                    }
                }
                /*if (animation.AnimationNumber == 7 + Entity.type)
                    if (SDL_HasRectIntersectionFloat(&Entity.dest, &animation.destRect)) {
                        animation.destRect = Entity.dest;
                }*/
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
    for (auto& Part : Loot::ParticleVector)
    {
        SDL_SetRenderDrawColor(Setup::renderer, Part.color.r, Part.color.g, Part.color.b, Part.color.a);
        SDL_RenderFillRect(Setup::renderer, &Part.dst);
        SDL_SetRenderDrawColor(Setup::renderer, 0, 0, 0, 255);
        SDL_RenderRect(Setup::renderer, &Part.dst);
    }
    ScoreChange();
    CoinsChange();
    UI::RenderUI();
    SDL_RenderPresent(renderer);
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

