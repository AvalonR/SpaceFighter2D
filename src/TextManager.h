#ifndef TEXTMANAGER_H
#define TEXTMANAGER_H

#include "Setup.h"
#include <iostream>
#include <string>
#include <unordered_map>

class TextManager {
private:
  std::unordered_map<int, TTF_Font *> fontCache; // Cache fonts by size

  struct TextCacheKey {
    std::string text;
    int size;
    SDL_Color color;

    bool operator==(const TextCacheKey &other) const {
      return text == other.text && size == other.size &&
             color.r == other.color.r && color.g == other.color.g &&
             color.b == other.color.b && color.a == other.color.a;
    }
  };

  struct TextCacheKeyHash {
    std::size_t operator()(const TextCacheKey &k) const {
      return std::hash<std::string>()(k.text) ^
             (std::hash<int>()(k.size) << 1) ^
             (std::hash<int>()(k.color.r) << 2);
    }
  };

  std::unordered_map<TextCacheKey, SDL_Texture *, TextCacheKeyHash>
      textureCache;
  std::string fontPath;

  // Private constructor
  TextManager();

  // Delete copy constructor and assignment operator
  TextManager(const TextManager &) = delete;
  TextManager &operator=(const TextManager &) = delete;

  // Private helper methods
  TTF_Font *GetFont(int size);
  SDL_Texture *GetOrCreateTexture(const TextCacheKey &key, TTF_Font *font,
                                  const char *text, SDL_Color color);

  // Instance method that does the actual work
  void RenderTextInternal(const char *text, Vector Coordinates,
                          SDL_Color TextColor, int TextSize);

public:
  ~TextManager();

  // Static method - this is what you call
  static void RenderText(const char *text, Vector Coordinates,
                         SDL_Color TextColor, int TextSize);

  // Optional: Clear cache method
  static void ClearTextureCache();
};

#endif // TEXTMANAGER_H
