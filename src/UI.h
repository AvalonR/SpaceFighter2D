//
// Created by romak on 18.01.2025.
//

#ifndef UI_H
#define UI_H

#include <stack>
#include <string>

#include "Setup.h"

enum class UIState {
  MAIN_MENU,
  SWARM_MODE_PLAYING,
  PAUSE_MENU,
  SETTINGS_MENU,
  VIDEO_SETTINGS,
  AUDIO_SETTINGS,
  MISC_SETTINGS,
  DIFFICULTY_SELECTION,
  STORE_MENU,
  LEVEL_UP_MENU,
  RESTART_CONFIRMATION,
  SCORE_BOARD,
  ACHIEVEMENTS
};

class UI {
private:
  static std::stack<UIState> stateStack;

public:
  static void PushState(UIState newState) { stateStack.push(newState); }

  static void PopState() {
    if (!stateStack.empty()) {
      stateStack.pop();
    }
  }

  static UIState GetCurrentState() {
    return stateStack.empty() ? UIState::MAIN_MENU : stateStack.top();
  }

  static UIState GetPreviousState() {
    if (stateStack.size() < 2)
      return UIState::MAIN_MENU;

    auto temp = stateStack;
    temp.pop();
    return temp.top();
  }

  static bool ContainsSwarmPlaying() {
    auto temp = stateStack; // make a copy so we don’t modify the original
    while (!temp.empty()) {
      if (temp.top() == UIState::SWARM_MODE_PLAYING)
        return true;
      temp.pop();
    }
    return false;
  }

  static void ClearAndSet(UIState newState) {
    while (!stateStack.empty())
      stateStack.pop();
    stateStack.push(newState);
  }

  static void UpdateCurrentLayerS();
  static int Scrolling;
  static bool ControlsPressed[7];
  static bool DifficultyChosen;
  static bool StoreChosen;
  static bool LevelUpChosen;
  static void Init();
  static SDL_Texture *UITexture;
  static bool Button(SDL_FRect ButtonDimensions, SDL_Color ButtonColor,
                     const char *ButtonName, Vector TextDimensions,
                     int TextSize);
  static bool Button(SDL_FRect ButtonDimensions, SDL_Color ButtonColor,
                     const char *ButtonName, Vector TextDimensions,
                     int TextSize, SDL_Color TextColor);
  static int Dropdown(int choice_count, int current_choice,
                      SDL_FRect DropdownDimensions,
                      std::vector<const char *> DropdownOptions);
  static float Slider(SDL_FRect dest, float volume);
  static void RenderUI(GameManager &gm);
  static void PlayerStats(GameManager &gm);
  static void RestartRun(GameManager &gm);
  static void DifficultyChoice(GameManager &gm);
  static void NextLevelPopUp(GameManager &gm);
  static void Escape(GameManager &gm);
  static void ScoreBoard(GameManager &gm);
  static void Achievements(GameManager &gm);
  static void Store(GameManager &gm);
  static void LevelUp(GameManager &gm);
  static void CoinsChange(GameManager &gm);
  static void ScoreChange(GameManager &gm);
  static bool Main_Menu(GameManager &gm);
  static void Cleanup();
};

#endif // UI_H
