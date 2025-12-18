//
// Entity.h - Base Entity class for all game objects
//

#ifndef ENTITY_H
#define ENTITY_H

#include <SDL3/SDL.h>
#include <cstdint>

// Vector struct - used by Entity and throughout the codebase
struct Vector {
  float x, y;
};

// Forward declarations
class GameManager;

class Entity {
protected:
  float HP, BC;
  int type;
  float x, y;
  int TextureID{};
  SDL_FRect src, dest;
  double rotation;
  Vector velocity;
  float shooting_delay, effectTimer;
  uint32_t UID;

public:
  Entity();
  virtual ~Entity() = default;

  virtual void update(GameManager &gm) = 0;
  virtual void onDeath(GameManager &gm) = 0;
  virtual int getTypeID() const = 0;

  void takeDamage(float amount) { HP -= amount; }
  bool isDead() const { return HP <= 0; }

  float getHP() const { return HP; }
  void setHP(float hp) { HP = hp; }
  float getBC() const { return BC; }
  void setBC(float bc) { BC = bc; }
  SDL_FRect getDestRect() const { return dest; }
  SDL_FRect &getDestRectRef() { return dest; }
  SDL_FRect getSrcRect() const { return src; }
  SDL_FRect &getSrcRectRef() { return src; }
  uint32_t getUID() const { return UID; }
  void setUID(uint32_t id) { UID = id; }
  Vector getVelocity() const { return velocity; }
  void setVelocity(Vector v) { velocity = v; }
  double getRotation() const { return rotation; }
  void setRotation(double r) { rotation = r; }
  int getTextureID() const { return TextureID; }
  float getEffectTimer() const { return effectTimer; }
  void setEffectTimer(float timer) { effectTimer = timer; }
  float getX() const { return x; }
  float getY() const { return y; }
  float getShootingDelay() const { return shooting_delay; }
  void setShootingDelay(float delay) { shooting_delay = delay; }

  void setPosition(float px, float py) {
    x = px;
    y = py;
    dest.x = x;
    dest.y = y;
  }

  void updatePosition(double deltaTime, float speedMultiplier = 1.0f) {
    x += velocity.x * deltaTime * speedMultiplier;
    y += velocity.y * deltaTime * speedMultiplier;
    dest.x = x;
    dest.y = y;
  }
};

#endif // ENTITY_H
