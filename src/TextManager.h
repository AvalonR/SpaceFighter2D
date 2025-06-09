//
// Created by romak on 29.01.2025.
//

#ifndef TEXTMANAGER_H
#define TEXTMANAGER_H

#include "Setup.h"

class TextManager {
public:
    TTF_Font* font;
    SDL_Surface* textSurface;
    SDL_Texture* textTexture;
    static void RenderText(const char* text, Vector Coordinates, SDL_Color color, int TextSize);
};



#endif //TEXTMANAGER_H
