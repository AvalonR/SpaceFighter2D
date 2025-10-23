//
// Created by romak on 18.01.2025.
//

#include "UI.h"

#include <cmath>
#include <cstring>
#include <iostream>
#include <map>

#include "Enemy.h"
#include "Player.h"
#include "Setup.h"
#include "Sound.h"
#include "TextManager.h"
#include "TextureManager.h"

SDL_Texture *UI::UITexture = nullptr;
SDL_Texture *MainMenuBackground = nullptr;

int buttonCooldown = 0;
int UI::Scrolling = 0;
bool UI::ControlsPressed[7];

int LevelPopUp = -1;

std::multimap<UIState, std::string> ButtonLayoutS = {
    // MAIN_MENU
    {UIState::MAIN_MENU, "Swarm Mode"},
    {UIState::MAIN_MENU, "Restart Swarm Mode"},
    {UIState::MAIN_MENU, "Score Board"},
    {UIState::MAIN_MENU, "Achievements"},
    {UIState::MAIN_MENU, "Quit"},
    {UIState::MAIN_MENU, "Settings"},

    // PAUSE_MENU
    {UIState::SETTINGS_MENU, "Video"},
    {UIState::SETTINGS_MENU, "Audio"},
    {UIState::SETTINGS_MENU, "Misc"},
    {UIState::SETTINGS_MENU, "Back"},
    {UIState::SETTINGS_MENU, "Main Menu"},
    {UIState::SETTINGS_MENU, "FullScreen"},
    {UIState::SETTINGS_MENU, "Windowed"},
    {UIState::SETTINGS_MENU, "--------"},
    {UIState::SETTINGS_MENU, "1920x1080"},
    {UIState::SETTINGS_MENU, "1280x720"},

    // Various game states
    {UIState::RESTART_CONFIRMATION, "Restart"},
    {UIState::RESTART_CONFIRMATION, "Main Menu"},
    {UIState::DIFFICULTY_SELECTION, "Novice (1x)"},
    {UIState::DIFFICULTY_SELECTION, "Veteran (1.5x)"},
    {UIState::DIFFICULTY_SELECTION, "Calamity (2x)"},
    {UIState::DIFFICULTY_SELECTION, "Extremity (3x)"},
    {UIState::STORE_MENU, "Next Level"},
    {UIState::STORE_MENU, "Heal +20 Health"},
    {UIState::STORE_MENU, "Increase Max"},
    {UIState::STORE_MENU, "Boost Max"},
    {UIState::STORE_MENU, "Upgrade Bullet"},
    {UIState::STORE_MENU, "Multi-Shot:"},
    {UIState::STORE_MENU, "Unlock Shotgun"},
    {UIState::STORE_MENU, "Unlock Radial Fire"},
    {UIState::STORE_MENU, "Unlock Parallel"},
    {UIState::LEVEL_UP_MENU, "Battle-Hardened"},
    {UIState::LEVEL_UP_MENU, "Sharpened Aim"},
    {UIState::LEVEL_UP_MENU, "Efficient Repair"},
    {UIState::LEVEL_UP_MENU, "Thruster"},

    // Back buttons
    {UIState::SCORE_BOARD, "Back"},
    {UIState::ACHIEVEMENTS, "Back"}};

/*Debugging
std::string ToString(UIState state) {
  switch (state) {
  case UIState::MAIN_MENU:
    return "MAIN_MENU";
  case UIState::SWARM_MODE_PLAYING:
    return "SWARM_MODE_PLAYING";
  case UIState::PAUSE_MENU:
    return "PAUSE_MENU";
  case UIState::SETTINGS_MENU:
    return "SETTINGS_MENU";
  case UIState::VIDEO_SETTINGS:
    return "VIDEO_SETTINGS";
  case UIState::AUDIO_SETTINGS:
    return "AUDIO_SETTINGS";
  case UIState::MISC_SETTINGS:
    return "MISC_SETTINGS";
  case UIState::DIFFICULTY_SELECTION:
    return "DIFFICULTY_SELECTION";
  case UIState::STORE_MENU:
    return "STORE_MENU";
  case UIState::LEVEL_UP_MENU:
    return "LEVEL_UP_MENU";
  case UIState::RESTART_CONFIRMATION:
    return "RESTART_CONFIRMATION";
  case UIState::SCORE_BOARD:
    return "SCORE_BOARD";
  case UIState::ACHIEVEMENTS:
    return "ACHIEVEMENTS";
  default:
    return "UNKNOWN_STATE";
  }
}
*/

std::stack<UIState> UI::stateStack;
void UI::Init() {
  UITexture = SDL_CreateTexture(GameManagerSingleton::instance().getRenderer(),
                                SDL_PIXELFORMAT_RGBA8888,
                                SDL_TEXTUREACCESS_TARGET, 1920, 1080);
  if (!UITexture) {
    SDL_Log("Failed to create UI texture! SDL_Error: ", SDL_GetError());
  }

  SDL_SetTextureBlendMode(UITexture, SDL_BLENDMODE_BLEND);
  ClearAndSet(UIState::MAIN_MENU);
  MainMenuBackground = TextureManager::LoadTexture("background1 .png");
}
bool UI::DifficultyChosen = false;
bool UI::StoreChosen = true;
bool UI::LevelUpChosen = true;

void UI::RenderUI(GameManager &gm) {
  SDL_SetRenderTarget(gm.getRenderer(), UITexture);
  buttonCooldown--;

  SDL_SetRenderDrawBlendMode(gm.getRenderer(), SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(gm.getRenderer(), 0, 0, 0, 0);
  SDL_RenderClear(gm.getRenderer());
  UpdateCurrentLayerS();
  // std::cout << "Current state: " << ToString(UI::GetCurrentState()) <<
  // std::endl;
  if (!ContainsSwarmPlaying()) {
    SDL_RenderTexture(gm.getRenderer(), MainMenuBackground, nullptr, nullptr);
    if (Button({855, 500, 180, 35}, {138, 43, 226, 180}, "Swarm Mode",
               {860, 500}, 24)) {
      PushState(UIState::SWARM_MODE_PLAYING);
      gm.keepScore();
      if (!DifficultyChosen) {
        PushState(UIState::DIFFICULTY_SELECTION);
        return;
      } else if (!StoreChosen) {
        PushState(UIState::STORE_MENU);
        return;
      } else if (!LevelUpChosen) {
        PushState(UIState::LEVEL_UP_MENU);
        return;
      }
    }
    if (gm.getScore() > 0 || DifficultyChosen) {
      if (Button({805, 600, 290, 35}, {138, 43, 226, 180}, "Restart Swarm Mode",
                 {810, 600}, 24)) {
        Enemy::RestartWaves();
        gm.setRestart(true);
        PushState(UIState::SWARM_MODE_PLAYING);
        PushState(UIState::DIFFICULTY_SELECTION);
      }
    }
    if (Button({855, 920, 180, 35}, {138, 43, 226, 180}, "Quit", {860, 920},
               24)) {
      gm.setloopRunning(false);
      return;
    }
    if (Button({855, 750, 200, 35}, {138, 43, 226, 180}, "Score Board",
               {860, 750}, 24) ||
        GetCurrentState() == UIState::SCORE_BOARD) {
      if (GetCurrentState() != UIState::SCORE_BOARD) {
        PushState(UIState::SCORE_BOARD);
      }
      ScoreBoard(gm);
    }
    if (Button({855, 800, 200, 35}, {138, 43, 226, 180}, "Achievements",
               {860, 800}, 24) ||
        GetCurrentState() == UIState::ACHIEVEMENTS) {
      if (GetCurrentState() != UIState::ACHIEVEMENTS) {
        PushState(UIState::ACHIEVEMENTS);
      }
      Achievements(gm);
    }
    if (Button({855, 850, 180, 35}, {138, 43, 226, 180}, "Settings", {860, 850},
               24) ||
        GetCurrentState() == UIState::SETTINGS_MENU) {
      if (GetCurrentState() != UIState::SETTINGS_MENU) {
        PushState(UIState::SETTINGS_MENU);
      }
      Escape(gm);
    }
    SDL_FRect Buttons = {1600, 880, 64, 64};
    if (ControlsPressed[0]) {
      TextureManager::DrawTextureNP(27, gm.getRenderer(), &Buttons, 0);
    } else {
      TextureManager::DrawTextureNP(28, gm.getRenderer(), &Buttons, 0);
    }
    Buttons = {1660, 880, 64, 64};
    if (ControlsPressed[1]) {
      TextureManager::DrawTextureNP(31, gm.getRenderer(), &Buttons, 0);
    } else {
      TextureManager::DrawTextureNP(32, gm.getRenderer(), &Buttons, 0);
    }
    Buttons = {1720, 880, 64, 64};
    if (ControlsPressed[2]) {
      TextureManager::DrawTextureNP(25, gm.getRenderer(), &Buttons, 0);
    } else {
      TextureManager::DrawTextureNP(26, gm.getRenderer(), &Buttons, 0);
    }
    Buttons = {1610, 940, 64, 64};
    if (ControlsPressed[3]) {
      TextureManager::DrawTextureNP(35, gm.getRenderer(), &Buttons, 0);
    } else {
      TextureManager::DrawTextureNP(36, gm.getRenderer(), &Buttons, 0);
    }
    Buttons = {1670, 940, 64, 64};
    if (ControlsPressed[4]) {
      TextureManager::DrawTextureNP(33, gm.getRenderer(), &Buttons, 0);
    } else {
      TextureManager::DrawTextureNP(34, gm.getRenderer(), &Buttons, 0);
    }
    Buttons = {1730, 940, 64, 64};
    if (ControlsPressed[5]) {
      TextureManager::DrawTextureNP(37, gm.getRenderer(), &Buttons, 0);
    } else {
      TextureManager::DrawTextureNP(38, gm.getRenderer(), &Buttons, 0);
    }
    Buttons = {1810, 910, 64, 64};
    TextureManager::DrawTextureNP(39, gm.getRenderer(), &Buttons, 0);
  }
  if (ContainsSwarmPlaying()) {
    PlayerStats(gm);
    if (LevelPopUp >= 0) {
      NextLevelPopUp(gm);
    }
    if (GetCurrentState() == UIState::RESTART_CONFIRMATION) {
      gm.set_is_Paused(true);
      RestartRun(gm);
    }
    if (GetCurrentState() == UIState::STORE_MENU) {
      gm.set_is_Paused(true);
      Store(gm);
    }
    if (GetCurrentState() == UIState::LEVEL_UP_MENU) {
      gm.set_is_Paused(true);
      LevelUp(gm);
    }
    if (GetCurrentState() == UIState::DIFFICULTY_SELECTION) {
      gm.set_is_Paused(true);
      DifficultyChoice(gm);
    }
    if (GetCurrentState() == UIState::SWARM_MODE_PLAYING) {
      gm.set_is_Paused(false);
    }
    if (GetCurrentState() == UIState::SETTINGS_MENU) {
      gm.set_is_Paused(true);
      Escape(gm);
    }
  }

  std::string fps =
      "FPS: " + std::to_string(GameManagerSingleton::instance().getFPS());
  TextManager::RenderText(fps.c_str(), {0, 0}, {0, 255, 0, 255}, 20);
  std::string delta =
      "Delta: " + std::to_string(GameManagerSingleton::instance().getDelta());
  TextManager::RenderText(delta.c_str(), {100, 0}, {0, 255, 0, 255}, 20);

  SDL_SetRenderTarget(gm.getRenderer(), nullptr);
  SDL_RenderTexture(gm.getRenderer(), UITexture, nullptr, nullptr);
  if (GetCurrentState() == UIState::MAIN_MENU) {
    SDL_RenderPresent(gm.getRenderer());
  }
}

void UI::PlayerStats(GameManager &gm) {
  if (Player::PlayerUpgrades.TotalHP > 1.0f) {
    int numberOfSectors =
        std::max(1, static_cast<int>(ceil(Player::PlayerUpgrades.TotalHP)));
    int sizeOfSectors = 200 / numberOfSectors;

    float hp = std::clamp(gm.getEntityManager().getPlayer().HP, 0.0f,
                          static_cast<float>(numberOfSectors));

    SDL_FRect HPbar = {1700, 1040, 200, 20};
    SDL_FRect InnerHPBar = {1700, 1040, 200 * (hp / numberOfSectors), 20};

    SDL_SetRenderDrawColor(gm.getRenderer(), 70, 70, 70, 235);
    SDL_RenderFillRect(gm.getRenderer(), &HPbar);
    SDL_SetRenderDrawColor(gm.getRenderer(), 255, 0, 0, 255);
    SDL_RenderRect(gm.getRenderer(), &HPbar);
    SDL_RenderFillRect(gm.getRenderer(), &InnerHPBar);
    SDL_SetRenderDrawColor(gm.getRenderer(), 0, 0, 0, 255);
    for (int i = 1; i < numberOfSectors; i++) {
      SDL_RenderLine(gm.getRenderer(), 1700 + (sizeOfSectors * i), 1042,
                     1700 + (sizeOfSectors * i), 1058);
    }
  } else {
    SDL_FRect HPbar = {1700, 1040, 200, 20};
    SDL_FRect InnerHPBar = {1700, 1040,
                            200 * gm.getEntityManager().getPlayer().HP, 20};
    SDL_SetRenderDrawColor(gm.getRenderer(), 70, 70, 70, 235);
    SDL_RenderFillRect(gm.getRenderer(), &HPbar);
    SDL_SetRenderDrawColor(gm.getRenderer(), 255, 0, 0, 255);
    SDL_RenderRect(gm.getRenderer(), &HPbar);
    SDL_RenderFillRect(gm.getRenderer(), &InnerHPBar);
  }
  if (Player::PlayerUpgrades.TotalBulletCap > 1.0f) {
    int numberOfBulletSectors = std::max(
        1, static_cast<int>(ceil(Player::PlayerUpgrades.TotalBulletCap)));
    int sizeOfBulletSectors = 200 / numberOfBulletSectors;

    float bc = std::clamp(gm.getEntityManager().getPlayer().BC, 0.0f,
                          static_cast<float>(numberOfBulletSectors));

    SDL_FRect BulletCap = {220, 1040, -200, 20};
    SDL_FRect InnerBulletCap = {220, 1040, -200 * (bc / numberOfBulletSectors),
                                20};

    SDL_SetRenderDrawColor(gm.getRenderer(), 70, 70, 70, 235);
    SDL_RenderFillRect(gm.getRenderer(), &BulletCap);
    SDL_SetRenderDrawColor(gm.getRenderer(), 255, 255, 0, 255);
    SDL_RenderRect(gm.getRenderer(), &BulletCap);
    SDL_RenderFillRect(gm.getRenderer(), &InnerBulletCap);
    SDL_SetRenderDrawColor(gm.getRenderer(), 0, 0, 0, 255);
    for (int i = 1; i < numberOfBulletSectors; ++i) {
      SDL_RenderLine(gm.getRenderer(), 220 - (sizeOfBulletSectors * i), 1042,
                     220 - (sizeOfBulletSectors * i), 1058);
    }
  } else {
    SDL_FRect BulletCap = {220, 1040, -200, 20};
    SDL_FRect InnerBulletCap = {
        220, 1040, -200 * gm.getEntityManager().getPlayer().BC, 20};

    SDL_SetRenderDrawColor(gm.getRenderer(), 70, 70, 70, 235);
    SDL_RenderFillRect(gm.getRenderer(), &BulletCap);
    SDL_SetRenderDrawColor(gm.getRenderer(), 255, 255, 0, 255);
    SDL_RenderRect(gm.getRenderer(), &BulletCap);
    SDL_RenderFillRect(gm.getRenderer(), &InnerBulletCap);
  }
  SDL_FRect EXPbar = {20, 10, 1880, 30};
  SDL_FRect InnerEXPbar = {20, 10, 1880 * Player::PlayerUpgrades.ExperienceP,
                           30};

  SDL_SetRenderDrawColor(gm.getRenderer(), 70, 70, 70, 235);
  SDL_RenderFillRect(gm.getRenderer(), &EXPbar);
  SDL_SetRenderDrawColor(gm.getRenderer(), 0, 255, 255, 255);
  SDL_RenderRect(gm.getRenderer(), &EXPbar);
  SDL_RenderFillRect(gm.getRenderer(), &InnerEXPbar);

  std::string stageCount = "Level: " + std::to_string(gm.getCurrentLevel());
  TextManager::RenderText(stageCount.c_str(), {50, 60}, {255, 255, 255, 255},
                          24);

  std::string score =
      "Score: " + std::to_string(GameManagerSingleton::instance().getScore());
  TextManager::RenderText(score.c_str(), {640, 60}, {255, 255, 255, 255}, 24);

  std::string coins = "Coins: " + std::to_string(gm.getCoins());
  TextManager::RenderText(coins.c_str(), {1040, 60}, {255, 255, 255, 255}, 24);

  std::string entityCount =
      "Enemies: " +
      std::to_string(gm.getEntityManager().getEntities().size() - 1);
  TextManager::RenderText(entityCount.c_str(), {1740, 60}, {255, 255, 255, 255},
                          24);

  SDL_SetRenderDrawColor(gm.getRenderer(), 0, 0, 0, 255);
  SDL_FRect HPIcon = {230, 1032, 32, 32};
  TextureManager::DrawTextureNP(21, gm.getRenderer(), &HPIcon, 0);
  SDL_FRect BCIcon = {1662, 1032, 32, 32};
  TextureManager::DrawTextureNP(12, gm.getRenderer(), &BCIcon, 0);
}

bool video_settings = false;
bool audio_settings = false;
bool misc_settings = false;

void UI::RestartRun(GameManager &gm) {
  SDL_FRect RestartOutline = {700, 400, 500, 250};
  SDL_SetRenderDrawColor(gm.getRenderer(), 0, 0, 0, 255);
  SDL_RenderFillRect(gm.getRenderer(), &RestartOutline);
  SDL_SetRenderDrawColor(gm.getRenderer(), 0, 255, 255, 255);
  SDL_RenderRect(gm.getRenderer(), &RestartOutline);
  if (Button({865, 450, 155, 35}, {138, 43, 226, 180}, "Restart", {870, 450},
             24)) {
    Sound::RandomizeMusic();
    gm.setRestart(true);
    PopState();
    PushState(UIState::DIFFICULTY_SELECTION);
  }
  if (Button({865, 550, 155, 35}, {138, 43, 226, 180}, "Main Menu", {870, 550},
             24)) {
    gm.setRestart(true);
    ClearAndSet(UIState::MAIN_MENU);
  }
}

void UI::DifficultyChoice(GameManager &gm) {
  SDL_FRect RestartOutline = {700, 400, 500, 400};
  SDL_SetRenderDrawColor(gm.getRenderer(), 0, 0, 0, 255);
  SDL_RenderFillRect(gm.getRenderer(), &RestartOutline);
  SDL_SetRenderDrawColor(gm.getRenderer(), 0, 255, 255, 255);
  SDL_RenderRect(gm.getRenderer(), &RestartOutline);
  if (Button({845, 430, 205, 35}, {138, 43, 226, 180}, "Novice (1x)",
             {850, 430}, 24, {255, 255, 255, 255})) {
    gm.setDifficulty(1.0f);
    PopState();
    DifficultyChosen = true;
    LevelPopUp++;
  }
  if (Button({845, 530, 205, 35}, {138, 43, 226, 180}, "Veteran (1.5x)",
             {850, 530}, 24, {255, 200, 200, 255})) {
    gm.setDifficulty(1.5f);
    PopState();
    DifficultyChosen = true;
    LevelPopUp++;
  }
  if (Button({845, 630, 205, 35}, {138, 43, 226, 180}, "Calamity (2x)",
             {850, 630}, 24, {255, 120, 120, 255})) {
    gm.setDifficulty(2.0f);
    PopState();
    DifficultyChosen = true;
    LevelPopUp++;
  }
  if (Button({845, 730, 205, 35}, {138, 43, 226, 180}, "Extremity (3x)",
             {850, 730}, 24, {255, 0, 0, 255})) {
    gm.setDifficulty(3.0f);
    PopState();
    DifficultyChosen = true;
    LevelPopUp++;
  }
}

void UI::NextLevelPopUp(GameManager &gm) {
  if (LevelPopUp >= 250) {
    LevelPopUp = -1;
    return;
  }
  LevelPopUp++;
  SDL_FRect PopUPOutline = {700, 440, 500, 120};
  SDL_SetRenderDrawColor(gm.getRenderer(), 0, 0, 0, 250 - LevelPopUp);
  SDL_RenderFillRect(gm.getRenderer(), &PopUPOutline);
  SDL_SetRenderDrawColor(gm.getRenderer(), 0, 255, 255, 255 - LevelPopUp);
  SDL_RenderRect(gm.getRenderer(), &PopUPOutline);
  std::string text = "Current Level: " + std::to_string(gm.getCurrentLevel());
  TextManager::RenderText(text.c_str(), {820, 480},
                          {255, 255, 255, static_cast<Uint8>(255 - LevelPopUp)},
                          28);
}
void UI::Achievements(GameManager &gm) {
  const int entryHeight = 60;
  const int maxVisibleEntries = 5;
  const int totalEntries = 15;
  SDL_FRect SettingsOutline = {500, 300, 900, 500};
  SDL_SetRenderDrawColor(gm.getRenderer(), 0, 0, 0, 255);
  SDL_RenderFillRect(gm.getRenderer(), &SettingsOutline);
  SDL_SetRenderDrawColor(gm.getRenderer(), 255, 255, 255, 255);
  SDL_RenderRect(gm.getRenderer(), &SettingsOutline);
  int firstIndex = Scrolling / entryHeight;
  int lastIndex = std::min(firstIndex + maxVisibleEntries, totalEntries);

  int maxScroll = std::max(0, (totalEntries - maxVisibleEntries) * entryHeight);
  Scrolling = std::clamp(Scrolling, 0, maxScroll);
  SDL_FRect rectAround = {590, 300, 750, 55};
  for (int i = firstIndex; i < lastIndex; i++) {
    float yOffset =
        350.0f - (Scrolling % entryHeight) + ((i - firstIndex) * entryHeight);
    rectAround.y = yOffset;

    if (Player::Achievements[i].achieved) {
      SDL_RenderRect(gm.getRenderer(), &rectAround);
      std::string score =
          std::to_string(i + 1) + " " + Player::Achievements[i].title;
      TextManager::RenderText(score.c_str(), {600, yOffset},
                              {255, 255, 255, 255}, 24);
      TextManager::RenderText(Player::Achievements[i].description.c_str(),
                              {600, yOffset + 25}, {255, 255, 255, 255}, 24);
    } else {
      SDL_RenderRect(gm.getRenderer(), &rectAround);
      std::string questionedTitle(Player::Achievements[i].title.length(), '?');
      std::string score = std::to_string(i + 1) + " " + questionedTitle;
      TextManager::RenderText(score.c_str(), {600, yOffset},
                              {255, 255, 255, 255}, 24);
    }
    if (totalEntries > maxVisibleEntries) {
      float scrollbarHeight =
          SettingsOutline.h * ((float)maxVisibleEntries / totalEntries);
      float scrollbarY =
          SettingsOutline.y + (float(Scrolling) / maxScroll) *
                                  (SettingsOutline.h - scrollbarHeight);

      SDL_FRect scrollbar = {SettingsOutline.x + SettingsOutline.w - 10,
                             scrollbarY, 6, scrollbarHeight};

      SDL_SetRenderDrawColor(gm.getRenderer(), 180, 180, 180, 255);
      SDL_RenderFillRect(gm.getRenderer(), &scrollbar);
    }
  }
  if (Button({595, 700, 76, 35}, {255, 140, 66, 180}, "Back", {600, 700}, 24)) {
    PopState();
  }
}
void UI::ScoreBoard(GameManager &gm) {
  const int entryHeight = 30;
  int maxVisibleEntries = 10;
  if (Setup::ScoreHistory.size() < 11) {
    maxVisibleEntries = Setup::ScoreHistory.size();
  }
  const int totalEntries = Setup::ScoreHistory.size();
  SDL_FRect SettingsOutline = {500, 300, 900, 500};
  SDL_SetRenderDrawColor(gm.getRenderer(), 0, 0, 0, 255);
  SDL_RenderFillRect(gm.getRenderer(), &SettingsOutline);
  SDL_SetRenderDrawColor(gm.getRenderer(), 255, 255, 255, 255);
  SDL_RenderRect(gm.getRenderer(), &SettingsOutline);
  int firstIndex = Scrolling / entryHeight;
  int lastIndex = std::min(firstIndex + maxVisibleEntries, totalEntries);

  int maxScroll = std::max(0, (totalEntries - maxVisibleEntries) * entryHeight);
  Scrolling = std::clamp(Scrolling, 0, maxScroll);
  for (int i = firstIndex; i < lastIndex; i++) {
    float yOffset =
        350.0f - (Scrolling % entryHeight) + ((i - firstIndex) * entryHeight);
    std::string score = std::to_string(i + 1) + " ------------- " +
                        std::to_string(Setup::ScoreHistory[i]);
    TextManager::RenderText(score.c_str(), {600, yOffset}, {255, 255, 255, 255},
                            24);
  }
  if (totalEntries > maxVisibleEntries) {
    float scrollbarHeight =
        SettingsOutline.h * ((float)maxVisibleEntries / totalEntries);
    float scrollbarY =
        SettingsOutline.y +
        (float(Scrolling) / maxScroll) * (SettingsOutline.h - scrollbarHeight);

    SDL_FRect scrollbar = {SettingsOutline.x + SettingsOutline.w - 10,
                           scrollbarY, 6, scrollbarHeight};

    SDL_SetRenderDrawColor(gm.getRenderer(), 180, 180, 180, 255);
    SDL_RenderFillRect(gm.getRenderer(), &scrollbar);
  }
  if (Button({595, 700, 76, 35}, {255, 140, 66, 180}, "Back", {600, 700}, 24)) {
    PopState();
  }
}
void UI::Store(GameManager &gm) {
  SDL_FRect StoreOutline = {500, 300, 900, 500};
  SDL_SetRenderDrawColor(gm.getRenderer(), 0, 0, 0, 180);
  SDL_RenderFillRect(gm.getRenderer(), &StoreOutline);
  SDL_SetRenderDrawColor(gm.getRenderer(), 255, 255, 255, 255);
  SDL_RenderRect(gm.getRenderer(), &StoreOutline);
  int TargetCoins = gm.getTargetCoins();

  SDL_FRect BCIcon = {580, 340, 128, 128};
  if (TargetCoins < 80) {
    TextureManager::DrawTextureNP(40, gm.getRenderer(), &BCIcon, 0);
    TextManager::RenderText("Unavailable", {550, 470}, {255, 255, 255, 255},
                            16);
  } else {
    if (Button({550, 350, 180, 165}, {255, 255, 255, 100}, "Heal +20 Health",
               {555, 470}, 16)) {
      gm.spendCoins(80);
      gm.getEntityManager().getPlayer().HP =
          std::min(gm.getEntityManager().getPlayer().HP + 0.2f,
                   Player::PlayerUpgrades.TotalHP);
      Sound::PlaySound(6);
    }
    TextureManager::DrawTextureNP(9, gm.getRenderer(), &BCIcon, 0);
  }
  TextManager::RenderText("Price: 80", {550, 350}, {255, 50, 50, 255}, 14);

  BCIcon = {790, 340, 128, 128};
  if (TargetCoins < 100) {
    TextureManager::DrawTextureNP(40, gm.getRenderer(), &BCIcon, 0);
    TextManager::RenderText("Unavailable", {760, 470}, {255, 255, 255, 255},
                            16);
  } else {
    if (Button({760, 350, 180, 165}, {255, 255, 255, 100}, "Increase Max",
               {765, 470}, 16)) {
      gm.spendCoins(100);
      Player::PlayerUpgrades.TotalHP += 0.2f;
      gm.getEntityManager().getPlayer().HP =
          std::min(gm.getEntityManager().getPlayer().HP + 0.2f,
                   Player::PlayerUpgrades.TotalHP);
      Sound::PlaySound(6);
    }
    TextManager::RenderText("Health +20", {765, 490}, {255, 255, 255, 255}, 16);
    TextureManager::DrawTextureNP(15, gm.getRenderer(), &BCIcon, 0);
  }
  TextManager::RenderText("Price: 100", {760, 350}, {255, 50, 50, 255}, 14);

  BCIcon = {1000, 340, 128, 128};
  if (TargetCoins < 80) {
    TextureManager::DrawTextureNP(40, gm.getRenderer(), &BCIcon, 0);
    TextManager::RenderText("Unavailable", {970, 470}, {255, 255, 255, 255},
                            16);
  } else {
    if (Button({970, 350, 180, 165}, {255, 255, 255, 100}, "Boost Max",
               {975, 470}, 16)) {
      gm.spendCoins(80);
      Player::PlayerUpgrades.TotalBulletCap += 0.2f;
      gm.getEntityManager().getPlayer().BC =
          std::min(gm.getEntityManager().getPlayer().BC + 0.2f,
                   Player::PlayerUpgrades.TotalBulletCap);
      Sound::PlaySound(6);
    }
    TextManager::RenderText("Ammo +20", {975, 490}, {255, 255, 255, 255}, 16);
    TextureManager::DrawTextureNP(19, gm.getRenderer(), &BCIcon, 0);
  }
  TextManager::RenderText("Price: 80", {970, 350}, {255, 50, 50, 255}, 14);

  BCIcon = {1210, 340, 128, 128};
  if (TargetCoins < 150) {
    TextureManager::DrawTextureNP(40, gm.getRenderer(), &BCIcon, 0);
    TextManager::RenderText("Unavailable", {1185, 470}, {255, 255, 255, 255},
                            16);
  } else {
    if (Button({1180, 350, 180, 165}, {255, 255, 255, 100}, "Upgrade Bullet",
               {1185, 470}, 16)) {
      gm.spendCoins(150);
      Player::PlayerUpgrades.damagePerBullet += 0.05f;
      Sound::PlaySound(6);
    }
    TextManager::RenderText("Damage +5", {1185, 490}, {255, 255, 255, 255}, 16);
    TextureManager::DrawTextureNP(17, gm.getRenderer(), &BCIcon, 0);
  }
  TextManager::RenderText("Price: 150", {1180, 350}, {255, 50, 50, 255}, 14);

  BCIcon = {580, 544, 128, 128};
  if (TargetCoins < 200) {
    TextureManager::DrawTextureNP(40, gm.getRenderer(), &BCIcon, 0);
    TextManager::RenderText("Unavailable", {550, 674}, {255, 255, 255, 255},
                            16);
  } else {
    if (Button({550, 554, 180, 165}, {255, 255, 255, 100},
               "Multi-Shot:", {555, 660}, 16)) {
      Player::upgradedPlayer = true;
      gm.spendCoins(200);
      Player::PlayerUpgrades.extraBulletsPerShot += 1;
      Sound::PlaySound(6);
    }
    TextManager::RenderText("+1 Bullet", {555, 680}, {255, 255, 255, 255}, 16);
    TextureManager::DrawTextureNP(19, gm.getRenderer(), &BCIcon, 0);
    BCIcon = {595, 595, 32, 32};
    TextureManager::DrawTextureNP(9, gm.getRenderer(), &BCIcon, 0);
  }
  TextManager::RenderText("Price: 200", {550, 554}, {255, 50, 50, 255}, 14);

  BCIcon = {790, 544, 128, 128};
  if (TargetCoins < 300 || Player::PlayerUpgrades.firemodes[0]) {
    TextureManager::DrawTextureNP(40, gm.getRenderer(), &BCIcon, 0);
    TextManager::RenderText("Unavailable", {760, 674}, {255, 255, 255, 255},
                            16);
  } else {
    if (Button({760, 554, 180, 165}, {255, 255, 255, 100}, "Unlock Shotgun",
               {765, 660}, 16)) {
      Player::upgradedPlayer = true;
      gm.spendCoins(300);
      Player::PlayerUpgrades.firemodes[0] = true;
      Sound::PlaySound(6);
    }
    TextManager::RenderText("Spread", {765, 680}, {255, 255, 255, 255}, 16);
    TextureManager::DrawTextureNP(18, gm.getRenderer(), &BCIcon, 0);
  }
  TextManager::RenderText("Price: 300", {760, 554}, {255, 50, 50, 255}, 14);

  BCIcon = {1000, 544, 128, 128};
  if (TargetCoins < 300 || Player::PlayerUpgrades.firemodes[1]) {
    TextureManager::DrawTextureNP(40, gm.getRenderer(), &BCIcon, 0);
    TextManager::RenderText("Unavailable", {970, 674}, {255, 255, 255, 255},
                            16);
  } else {
    if (Button({970, 554, 180, 165}, {255, 255, 255, 100}, "Unlock Radial Fire",
               {975, 660}, 16)) {
      Player::upgradedPlayer = true;
      gm.spendCoins(300);
      Player::PlayerUpgrades.firemodes[1] = true;
      Sound::PlaySound(6);
    }
    TextureManager::DrawTextureNP(16, gm.getRenderer(), &BCIcon, 0);
  }
  TextManager::RenderText("Price: 300", {970, 554}, {255, 50, 50, 255}, 14);

  BCIcon = {1210, 544, 128, 128};
  if (TargetCoins < 300 || Player::PlayerUpgrades.firemodes[2]) {
    TextureManager::DrawTextureNP(40, gm.getRenderer(), &BCIcon, 0);
    TextManager::RenderText("Unavailable", {1180, 674}, {255, 255, 255, 255},
                            16);
  } else {
    if (Button({1180, 554, 180, 165}, {255, 255, 255, 100}, "Unlock Parallel",
               {1185, 660}, 16)) {
      Player::upgradedPlayer = true;
      gm.spendCoins(300);
      Player::PlayerUpgrades.firemodes[2] = true;
      Sound::PlaySound(6);
    }
    TextManager::RenderText("Fire", {1185, 680}, {255, 255, 255, 255}, 16);
    TextureManager::DrawTextureNP(19, gm.getRenderer(), &BCIcon, 0);
  }
  TextManager::RenderText("Price: 300", {1180, 554}, {255, 50, 50, 255}, 14);
  if (Button({1200, 740, 150, 35}, {255, 140, 66, 180}, "Next Level",
             {1205, 740}, 24)) {
    gm.set_is_Paused(false);
    PopState();
    StoreChosen = true;
    NextLevelPopUp(gm);
  }
}
void UI::LevelUp(GameManager &gm) {
  SDL_FRect LevelOutline = {500, 300, 900, 500};
  SDL_SetRenderDrawColor(gm.getRenderer(), 0, 0, 0, 180);
  SDL_RenderFillRect(gm.getRenderer(), &LevelOutline);
  SDL_SetRenderDrawColor(gm.getRenderer(), 255, 255, 255, 255);
  SDL_RenderRect(gm.getRenderer(), &LevelOutline);
  if (Button({550, 400, 180, 260}, {255, 255, 255, 100}, "Battle-Hardened",
             {555, 550}, 18)) {
    gm.set_is_Paused(false);
    PopState();
    LevelUpChosen = true;
    Player::PlayerUpgrades.ExperienceP = 0;
    Player::PlayerUpgrades.bulletCooldownMultiplier *= 0.9f;
    Player::levelUpSoundPlayed = false;
    Sound::PlaySound(11);
  }
  TextManager::RenderText("Loader", {555, 570}, {255, 255, 255, 255}, 18);
  TextManager::RenderText("Slightly Decrease", {555, 590}, {255, 255, 255, 255},
                          16);
  TextManager::RenderText("Bullet Cooldown", {555, 610}, {255, 255, 255, 255},
                          16);
  TextManager::RenderText("-10%", {555, 630}, {255, 255, 255, 255}, 16);
  SDL_FRect BCIcon = {580, 420, 128, 128};
  TextureManager::DrawTextureNP(13, gm.getRenderer(), &BCIcon, 0);
  if (Button({760, 400, 180, 260}, {255, 255, 255, 100}, "Sharpened Aim",
             {765, 550}, 18)) {
    gm.set_is_Paused(false);
    PopState();
    LevelUpChosen = true;
    Player::PlayerUpgrades.ExperienceP = 0;
    Player::PlayerUpgrades.damagePerBullet *= 1.15f;
    Player::levelUpSoundPlayed = false;
    Sound::PlaySound(11);
  }
  TextManager::RenderText("Slightly Increase", {765, 570}, {255, 255, 255, 255},
                          16);
  TextManager::RenderText("Bullet Damage", {765, 590}, {255, 255, 255, 255},
                          16);
  TextManager::RenderText("+15%", {765, 610}, {255, 255, 255, 255}, 16);
  BCIcon = {790, 420, 128, 128};
  TextureManager::DrawTextureNP(17, gm.getRenderer(), &BCIcon, 0);
  if (Button({970, 400, 180, 260}, {255, 255, 255, 100}, "Efficient Repair",
             {975, 550}, 18)) {
    gm.set_is_Paused(false);
    PopState();
    LevelUpChosen = true;
    Player::PlayerUpgrades.ExperienceP = 0;
    Player::PlayerUpgrades.PassiveHealing += 0.01f;
    Player::levelUpSoundPlayed = false;
    Sound::PlaySound(11);
  }
  TextManager::RenderText("Protocols", {975, 570}, {255, 255, 255, 255}, 16);
  TextManager::RenderText("Slight Passive", {975, 590}, {255, 255, 255, 255},
                          16);
  TextManager::RenderText("HP Regen +1%", {975, 610}, {255, 255, 255, 255}, 16);
  BCIcon = {1000, 420, 128, 128};
  TextureManager::DrawTextureNP(23, gm.getRenderer(), &BCIcon, 0);
  if (Button({1180, 400, 180, 260}, {255, 255, 255, 100}, "Thruster",
             {1185, 550}, 18)) {
    gm.set_is_Paused(false);
    PopState();
    LevelUpChosen = true;
    Player::PlayerUpgrades.ExperienceP = 0;
    Player::PlayerUpgrades.MovementSpeed *= 1.1f;
    Player::levelUpSoundPlayed = false;
    Sound::PlaySound(11);
  }
  TextManager::RenderText("Synchronization", {1185, 570}, {255, 255, 255, 255},
                          18);
  TextManager::RenderText("Slightly Increase", {1185, 590},
                          {255, 255, 255, 255}, 16);
  TextManager::RenderText("Movement Speed", {1185, 610}, {255, 255, 255, 255},
                          16);
  TextManager::RenderText("+10%", {1185, 630}, {255, 255, 255, 255}, 16);
  BCIcon = {1210, 420, 128, 128};
  TextureManager::DrawTextureNP(20, gm.getRenderer(), &BCIcon, 0);
}
void UI::Escape(GameManager &gm) {
  SDL_FRect SettingsOutline = {500, 300, 900, 500};
  SDL_SetRenderDrawColor(gm.getRenderer(), 0, 0, 0, 255);
  SDL_RenderFillRect(gm.getRenderer(), &SettingsOutline);
  SDL_SetRenderDrawColor(gm.getRenderer(), 255, 255, 255, 255);
  SDL_RenderRect(gm.getRenderer(), &SettingsOutline);
  if (!video_settings && !audio_settings && !misc_settings) {
    video_settings = Button({595, 400, 85, 35}, {255, 140, 66, 180}, "Video",
                            {600, 400}, 24);
    audio_settings = Button({595, 500, 85, 35}, {255, 140, 66, 180}, "Audio",
                            {600, 500}, 24);
    misc_settings =
        Button({595, 600, 73, 35}, {255, 140, 66, 180}, "Misc", {600, 600}, 24);
  }
  if (video_settings) {
    SDL_SetRenderDrawColor(gm.getRenderer(), 0, 0, 0, 255);
    SDL_FRect fillRect = {500, 300, 900, 500}; // clears the settings area
    SDL_RenderFillRect(gm.getRenderer(), &fillRect);

    SDL_SetRenderDrawColor(gm.getRenderer(), 255, 255, 255, 255);
    SDL_RenderRect(gm.getRenderer(), &SettingsOutline);
    TextManager::RenderText("Screen Mode: ", {700, 400}, {255, 255, 255, 255},
                            24);
    if (Button({1000, 400, 150, 50}, {200, 200, 200, 200}, "FullScreen",
               {1000, 400}, 24)) {
      SDL_SetWindowFullscreen(gm.getWindow(), true);
    }
    if (Button({1170, 400, 150, 50}, {200, 200, 200, 200}, "Windowed",
               {1170, 400}, 24)) {
      SDL_SetWindowFullscreen(gm.getWindow(), false);
    }
    TextManager::RenderText("Vsync: ", {700, 450}, {255, 255, 255, 255}, 24);
    TextManager::RenderText("Resolution: ", {700, 500}, {255, 255, 255, 255},
                            24);
    static int Dropdownchoice;
    if (gm.getWindowW() == 1920) {
      Dropdownchoice = 1;
    }
    Dropdownchoice = Dropdown(2, Dropdownchoice, {1000, 500, 200, 50},
                              {"--------", "1920x1080", "1280x720"});
    if (Dropdownchoice == 1 && gm.getWindowW() != 1920) {
      int WindowWidth = 1920;
      int WindowHeight = 1080;
      SDL_SetWindowSize(gm.getWindow(), WindowWidth, WindowHeight);
      SDL_RestoreWindow(gm.getWindow());
      SDL_ShowWindow(gm.getWindow());
      SDL_GetWindowSize(gm.getWindow(), &WindowWidth, &WindowHeight);
    } else if (Dropdownchoice == 2 && gm.getWindowW() != 1280) {
      int WindowWidth = 1280;
      int WindowHeight = 720;
      SDL_SetWindowSize(gm.getWindow(), WindowWidth, WindowHeight);
      SDL_RestoreWindow(gm.getWindow());
      SDL_ShowWindow(gm.getWindow());
      SDL_GetWindowSize(gm.getWindow(), &WindowWidth, &WindowHeight);
    }
  }
  if (audio_settings) {
    SDL_SetRenderDrawColor(gm.getRenderer(), 0, 0, 0, 255);
    SDL_FRect fillRect = {500, 300, 900, 500};
    SDL_RenderFillRect(gm.getRenderer(), &fillRect);
    SDL_SetRenderDrawColor(gm.getRenderer(), 255, 255, 255, 255);
    SDL_RenderRect(gm.getRenderer(), &SettingsOutline);

    TextManager::RenderText("Master Volume: ", {700, 400}, {255, 255, 255, 255},
                            24);
    float masterVolume =
        Slider({1000, 400, 200, 30}, Sound::VolumeAdjustmentChannel[0]);
    Sound::VolumeAdjustmentChannel[0] = masterVolume;
    MIX_SetMasterGain(Sound::mixer, Sound::VolumeAdjustmentChannel[0]);

    TextManager::RenderText("Music Volume: ", {700, 450}, {255, 255, 255, 255},
                            24);
    float musicVolume =
        Slider({1000, 450, 200, 30}, Sound::VolumeAdjustmentChannel[1]);
    Sound::VolumeAdjustmentChannel[1] = musicVolume;
    MIX_SetTagGain(Sound::mixer, Sound::TAG_MUSIC,
                   Sound::VolumeAdjustmentChannel[1]);

    TextManager::RenderText("UI Volume: ", {700, 500}, {255, 255, 255, 255},
                            24);
    float uiVolume =
        Slider({1000, 500, 200, 30}, Sound::VolumeAdjustmentChannel[2]);
    Sound::VolumeAdjustmentChannel[2] = uiVolume;
    MIX_SetTagGain(Sound::mixer, Sound::TAG_UI,
                   Sound::VolumeAdjustmentChannel[2]);

    TextManager::RenderText("Game Volume: ", {700, 550}, {255, 255, 255, 255},
                            24);
    float gameVolume =
        Slider({1000, 550, 200, 30}, Sound::VolumeAdjustmentChannel[3]);
    Sound::VolumeAdjustmentChannel[3] = gameVolume;
    MIX_SetTagGain(Sound::mixer, Sound::TAG_GAME,
                   Sound::VolumeAdjustmentChannel[3]);
  }
  if (misc_settings) {
    SDL_SetRenderDrawColor(gm.getRenderer(), 0, 0, 0, 255);
    SDL_FRect fillRect = {500, 300, 900, 500};
    SDL_RenderFillRect(gm.getRenderer(), &fillRect);

    SDL_SetRenderDrawColor(gm.getRenderer(), 255, 255, 255, 255);
    SDL_RenderRect(gm.getRenderer(), &SettingsOutline);
    TextManager::RenderText("-----", {700, 400}, {255, 255, 255, 255}, 24);
    TextManager::RenderText("-----", {700, 450}, {255, 255, 255, 255}, 24);
    TextManager::RenderText("-----", {700, 450}, {255, 255, 255, 255}, 24);
  }
  if (Button({595, 700, 76, 35}, {255, 140, 66, 180}, "Back", {600, 700}, 24)) {
    if (video_settings || audio_settings || misc_settings) {
      video_settings = false;
      audio_settings = false;
      misc_settings = false;
    } else {
      PopState();
    }
  }
  if (ContainsSwarmPlaying()) {
    if (Button({1145, 700, 150, 35}, {255, 140, 66, 180}, "Main Menu",
               {1150, 700}, 24)) {
      ClearAndSet(UIState::MAIN_MENU);
    }
  }
}

static std::unordered_set<std::string> currentLayerButtons;

void UI::UpdateCurrentLayerS() {
  currentLayerButtons.clear();
  auto range = ButtonLayoutS.equal_range(GetCurrentState());
  for (auto it = range.first; it != range.second; ++it) {
    currentLayerButtons.insert(it->second);
  }
}

bool LayerValidation(const char *ButtonName) {
  return currentLayerButtons.find(ButtonName) != currentLayerButtons.end();
}

bool UI::Button(SDL_FRect ButtonDimensions, SDL_Color ButtonColor,
                const char *ButtonName, Vector TextDimensions, int TextSize) {
  SDL_Renderer *renderer = GameManagerSingleton::instance().getRenderer();
  ButtonDimensions.y += 3.5;
  ButtonDimensions.x += 3.5;
  SDL_SetRenderDrawColor(renderer, 50, 50, 50, ButtonColor.a);
  SDL_RenderFillRect(renderer, &ButtonDimensions);

  ButtonDimensions.y -= 3.5;
  ButtonDimensions.x -= 3.5;
  SDL_SetRenderDrawColor(renderer, ButtonColor.r, ButtonColor.g, ButtonColor.b,
                         ButtonColor.a);
  SDL_RenderFillRect(renderer, &ButtonDimensions);

  TextManager::RenderText(ButtonName, {TextDimensions.x, TextDimensions.y},
                          {255, 255, 255, 255}, TextSize);
  SDL_FPoint LeftClickMousePos = {
      GameManagerSingleton::instance().getMouseCoordin().x,
      GameManagerSingleton::instance().getMouseCoordin().y};
  if (LayerValidation(ButtonName)) {
    if (SDL_PointInRectFloat(&LeftClickMousePos, &ButtonDimensions)) {
      SDL_SetRenderDrawColor(renderer, ButtonColor.r, ButtonColor.g,
                             ButtonColor.b, ButtonColor.a);
      SDL_RenderFillRect(renderer, &ButtonDimensions);

      SDL_SetRenderDrawColor(renderer, ButtonColor.r, ButtonColor.g,
                             ButtonColor.b, ButtonColor.a + 5);
      SDL_RenderRect(renderer, &ButtonDimensions);
      if (Player::LeftMouse && buttonCooldown <= 0) {
        buttonCooldown = 60;
        Sound::PlaySound(4);
        return true;
      }
    }
  }
  return false;
}

bool UI::Button(SDL_FRect ButtonDimensions, SDL_Color ButtonColor,
                const char *ButtonName, Vector TextDimensions, int TextSize,
                SDL_Color TextColor) {
  SDL_Renderer *renderer = GameManagerSingleton::instance().getRenderer();
  ButtonDimensions.y += 3.5;
  ButtonDimensions.x += 3.5;
  SDL_SetRenderDrawColor(renderer, 50, 50, 50, ButtonColor.a);
  SDL_RenderFillRect(renderer, &ButtonDimensions);

  ButtonDimensions.y -= 3.5;
  ButtonDimensions.x -= 3.5;
  SDL_SetRenderDrawColor(renderer, ButtonColor.r, ButtonColor.g, ButtonColor.b,
                         ButtonColor.a);
  SDL_RenderFillRect(renderer, &ButtonDimensions);

  TextManager::RenderText(ButtonName, {TextDimensions.x, TextDimensions.y},
                          TextColor, TextSize);
  SDL_FPoint LeftClickMousePos = {
      GameManagerSingleton::instance().getMouseCoordin().x,
      GameManagerSingleton::instance().getMouseCoordin().y};
  if (LayerValidation(ButtonName)) {
    if (SDL_PointInRectFloat(&LeftClickMousePos, &ButtonDimensions)) {
      SDL_SetRenderDrawColor(renderer, ButtonColor.r, ButtonColor.g,
                             ButtonColor.b, ButtonColor.a);
      SDL_RenderFillRect(renderer, &ButtonDimensions);

      SDL_SetRenderDrawColor(renderer, ButtonColor.r, ButtonColor.g,
                             ButtonColor.b, ButtonColor.a + 5);
      SDL_RenderRect(renderer, &ButtonDimensions);
      if (Player::LeftMouse && buttonCooldown <= 0) {
        buttonCooldown = 60;
        Sound::PlaySound(4);
        return true;
      }
    }
  }
  return false;
}

int UI::Dropdown(int choice_count, int current_choice,
                 SDL_FRect DropdownDimensions,
                 std::vector<const char *> DropdownOptions) {
  static bool opendropd;
  // ButtonLayout.insert(std::pair<int, const char*>(3,
  // DropdownOptions[current_choice]));
  SDL_FRect ButtonDim = DropdownDimensions;
  bool insideopendropd;
  if (!opendropd) {
    insideopendropd = Button(ButtonDim, {200, 200, 200, 200},
                             DropdownOptions[current_choice - 1],
                             {ButtonDim.x, ButtonDim.y}, 22);
  }
  if (insideopendropd) {
    opendropd = true;
  }
  if (opendropd) {
    SDL_SetRenderDrawColor(GameManagerSingleton::instance().getRenderer(), 0, 0,
                           0, 255);
    SDL_RenderFillRect(GameManagerSingleton::instance().getRenderer(),
                       &ButtonDim);
    for (int i = 0; i <= choice_count; i++) {
      ButtonDim.y = DropdownDimensions.y + (DropdownDimensions.h + 5) * i;

      if (Button(ButtonDim, {200, 200, 200, 200}, DropdownOptions[i],
                 {ButtonDim.x, ButtonDim.y}, 22)) {
        current_choice = i;
        opendropd = false;
      }
    }
  }
  return current_choice;
}

float UI::Slider(SDL_FRect dest, float volume) {
  SDL_SetRenderDrawColor(GameManagerSingleton::instance().getRenderer(), 255,
                         255, 255, 235);
  SDL_RenderRect(GameManagerSingleton::instance().getRenderer(), &dest);
  SDL_SetRenderDrawColor(GameManagerSingleton::instance().getRenderer(), 200,
                         200, 200, 235);
  SDL_FRect destFill = {dest.x, dest.y, dest.w * volume, dest.h};
  SDL_RenderFillRect(GameManagerSingleton::instance().getRenderer(), &destFill);
  SDL_SetRenderDrawColor(GameManagerSingleton::instance().getRenderer(), 255,
                         255, 255, 235);
  SDL_FRect sliderRect = {dest.x + dest.w * volume, dest.y - 5, dest.w / 20,
                          dest.h + 15};
  SDL_RenderFillRect(GameManagerSingleton::instance().getRenderer(),
                     &sliderRect);
  SDL_FPoint LeftMousePos = {
      GameManagerSingleton::instance().getMouseCoordin().x,
      GameManagerSingleton::instance().getMouseCoordin().y};
  if (SDL_PointInRectFloat(&LeftMousePos, &dest)) {
    if (Player::LeftMouse) {
      float calc = (LeftMousePos.x - dest.x) / dest.w;
      if (calc < 0) {
        calc = calc * -1;
      }
      calc = std::clamp(calc, 0.0f, 1.0f);
      volume = calc;
    }
  }
  return volume;
}

bool UI::Main_Menu(GameManager &gm) {
  if (GetCurrentState() == UIState::MAIN_MENU) {
    Player::playerInput(gm);
    gm.set_is_Paused(true);
    RenderUI(gm);
    return true;
  } else {
    return false;
  }
}

void UI::CoinsChange(GameManager &gm) {
  if (gm.isAnimatingCoins()) {
    std::string shownewcoins = "+" + std::to_string(gm.pendingCoinChange());
    TextManager::RenderText(shownewcoins.c_str(), {1200, 60},
                            {255, 255, 255, 255}, 24);
    gm.animateCoinsStep();
  }
}
void UI::ScoreChange(GameManager &gm) {

  if (gm.isAnimatingScore()) {
    std::string shownewscore = "+" + std::to_string(gm.pendingScoreChange());
    TextManager::RenderText(shownewscore.c_str(), {800, 60},
                            {255, 255, 255, 255}, 24);
    gm.animateScoreStep();
  }
}

void UI::Cleanup() {
  if (UITexture) {
    SDL_DestroyTexture(UITexture);
    UITexture = nullptr;
  }
}
