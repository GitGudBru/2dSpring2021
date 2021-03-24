#ifndef __ENEMY4_H__
#define __ENEMY4_H__

#include "entity.h"
#include "simple_json.h"

Entity *enemy4_spawn(Vector2D position);

void enemy4_update(Entity *self);

#endif