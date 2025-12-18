//
// Entity.cpp - Base Entity class implementation
//

#include "Entity.h"

Entity::Entity()
  : HP(1.0f), BC(1.0f), type(0), x(0.0f), y(0.0f),
    TextureID(0), src{}, dest{}, rotation(0.0),
    velocity{0.0f, 0.0f}, shooting_delay(0.0f),
    effectTimer(0.0f), UID(0) {
}
