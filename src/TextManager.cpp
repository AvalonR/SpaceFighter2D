#include "TextManager.h"
#include <cstring>

// Private constructor
TextManager::TextManager() {
  fontPath = Setup::basePath + "/assets/LiberationSans-Bold.ttf";
}

// Destructor
TextManager::~TextManager() {
  // Clean up all cached resources
  for (auto &pair : fontCache) {
    TTF_CloseFont(pair.second);
  }
  for (auto &pair : textureCache) {
    SDL_DestroyTexture(pair.second);
  }
}

// Static method - public interface
void TextManager::RenderText(const char *text, Vector Coordinates,
                             SDL_Color TextColor, int TextSize) {
  static TextManager instance; // Singleton instance created once
  instance.RenderTextInternal(text, Coordinates, TextColor, TextSize);
}

// Instance method - does the actual rendering
void TextManager::RenderTextInternal(const char *text, Vector Coordinates,
                                     SDL_Color TextColor, int TextSize) {
  // Get or load font
  TTF_Font *font = GetFont(TextSize);
  if (!font)
    return;

  // Check cache for existing texture
  TextCacheKey key{text, TextSize, TextColor};
  SDL_Texture *texture = GetOrCreateTexture(key, font, text, TextColor);
  if (!texture)
    return;

  // Get dimensions and render
  float textWidth = 0, textHeight = 0;
  SDL_GetTextureSize(texture, &textWidth, &textHeight);

  SDL_FRect destRect = {Coordinates.x, Coordinates.y, textWidth, textHeight};
  SDL_RenderTexture(GameManagerSingleton::instance().getRenderer(), texture,
                    nullptr, &destRect);
}

// Get or load font from cache
TTF_Font *TextManager::GetFont(int size) {
  auto it = fontCache.find(size);
  if (it != fontCache.end()) {
    return it->second;
  }

  TTF_Font *font = TTF_OpenFont(fontPath.c_str(), size);
  if (!font) {
    std::cerr << "Font could not be loaded! TTF_Error: " << SDL_GetError()
              << std::endl;
    return nullptr;
  }

  fontCache[size] = font;
  return font;
}

// Get or create texture from cache
SDL_Texture *TextManager::GetOrCreateTexture(const TextCacheKey &key,
                                             TTF_Font *font, const char *text,
                                             SDL_Color color) {
  auto it = textureCache.find(key);
  if (it != textureCache.end()) {
    return it->second;
  }

  // Create new texture
  SDL_Surface *surface = TTF_RenderText_Solid(font, text, strlen(text), color);
  if (!surface) {
    std::cerr << "Text surface could not be created! TTF_Error: "
              << SDL_GetError() << std::endl;
    return nullptr;
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(
      GameManagerSingleton::instance().getRenderer(), surface);
  SDL_DestroySurface(surface);

  if (!texture) {
    std::cerr << "Text texture could not be created! SDL_Error: "
              << SDL_GetError() << std::endl;
    return nullptr;
  }

  textureCache[key] = texture;
  return texture;
}

// Optional: Clear texture cache
void TextManager::ClearTextureCache() {
  static TextManager instance;
  for (auto &pair : instance.textureCache) {
    SDL_DestroyTexture(pair.second);
  }
  instance.textureCache.clear();
}
