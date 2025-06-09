//
// Created by romak on 29.01.2025.
//

#include "TextManager.h"
#include <iostream>

void TextManager::RenderText(const char* text, Vector Coordinates, SDL_Color TextColor, int TextSize)
{
    TextManager TextField;
    // Load a font (make sure you have a valid font file)
    TextField.font = TTF_OpenFont("C:/Windows/Fonts/ariblk.ttf", TextSize); // Adjust the font size as needed
    if (!TextField.font) {
        std::cerr << "Font could not be loaded! TTF_Error: " << SDL_GetError() << std::endl;
        return; // Exit if the font failed to load
    }

    TextField.textSurface = TTF_RenderText_Solid(TextField.font, text, strlen(text), TextColor);
    if (!TextField.textSurface) {
        std::cerr << "Text surface could not be created! TTF_Error: " << SDL_GetError() << std::endl;
        TTF_CloseFont(TextField.font); 
        return;
    }

    // Create the texture from the surface
    TextField.textTexture = SDL_CreateTextureFromSurface(Setup::renderer, TextField.textSurface);
    if (!TextField.textTexture) {
        std::cerr << "Text texture could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroySurface(TextField.textSurface); // Free the surface
        TTF_CloseFont(TextField.font);          // Free the font
        return;
    }

    // Free the surface since we no longer need it
    SDL_DestroySurface(TextField.textSurface);

    // Get the width and height of the text texture
    float textWidth = 0, textHeight = 0;
    SDL_GetTextureSize(TextField.textTexture, &textWidth, &textHeight);

    // Define the destination rectangle for rendering
    SDL_FRect TextRenderTarget = {Coordinates.x, Coordinates.y, textWidth, textHeight}; // Change position (x, y) as needed

    // Render the text texture to the screen
    SDL_RenderTexture(Setup::renderer, TextField.textTexture, nullptr, &TextRenderTarget);

    // Clean up
    SDL_DestroyTexture(TextField.textTexture);
    TTF_CloseFont(TextField.font);
}
