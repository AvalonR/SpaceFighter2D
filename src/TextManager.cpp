#include "TextManager.h"
#include <cstring>

TextManager::TextManager() {
  fontPath = Setup::basePath + "/assets/LiberationSans-Bold.ttf";
}

TextManager::~TextManager() {
  for (auto &pair : fontCache) {
    TTF_CloseFont(pair.second);
  }
  for (auto &pair : textureCache) {
    SDL_DestroyTexture(pair.second);
  }
}

void TextManager::RenderText(const char *text, Vector Coordinates,
                             SDL_Color TextColor, int TextSize) {
  static TextManager instance;
  instance.RenderTextInternal(text, Coordinates, TextColor, TextSize);
}

void TextManager::RenderTextInternal(const char *text, Vector Coordinates,
                                     SDL_Color TextColor, int TextSize) {
  TTF_Font *font = GetFont(TextSize);
  if (!font)
    return;

  TextCacheKey key{text, TextSize, TextColor};
  SDL_Texture *texture = GetOrCreateTexture(key, font, text, TextColor);
  if (!texture)
    return;

  float textWidth = 0, textHeight = 0;
  SDL_GetTextureSize(texture, &textWidth, &textHeight);

  SDL_FRect destRect = {Coordinates.x, Coordinates.y, textWidth, textHeight};
  SDL_RenderTexture(GameManagerSingleton::instance().getRenderer(), texture,
                    nullptr, &destRect);
}

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

SDL_Texture *TextManager::GetOrCreateTexture(const TextCacheKey &key,
                                             TTF_Font *font, const char *text,
                                             SDL_Color color) {
  auto it = textureCache.find(key);
  if (it != textureCache.end()) {
    return it->second;
  }

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

void TextManager::ClearTextureCache() {
  static TextManager instance;
  for (auto &pair : instance.textureCache) {
    SDL_DestroyTexture(pair.second);
  }
  instance.textureCache.clear();
}
