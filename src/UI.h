//
// Created by romak on 18.01.2025.
//

#ifndef UI_H
#define UI_H

#include <string>

#include "Setup.h"

class UI {
    public:
    static int CurrentLayer;
    static int UIAction;
    static int Scrolling;
    static bool ControlsPressed[7];
    static const char* CurrentMenu;
    static void Init();
    static SDL_Texture* UITexture, * FullEscapeTexture, * NoButtonEscapeTexture;
    static bool Button(SDL_FRect ButtonDimensions, SDL_Color ButtonColor, const char* ButtonName, Vector TextDimensions, int TextSize);
    static int Dropdown(int choice_count, int current_choice, SDL_FRect DropdownDimensions, std::vector<const char*> DropdownOptions);
    static int Slider(SDL_FRect dest, int volume);
    static void RenderUI();
    static void PlayerStats();
    static void RestartRun();
    static void NextLevelPopUp();
    static void Escape();
    static void ScoreBoard();
    static void Achievements();
    static void Store();
    static void LevelUp();
    static bool Main_Menu();
    static void Cleanup();
};



#endif //UI_H
