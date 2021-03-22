#ifndef __BOSS2_H__
#define __BOSS2_H__

#include "entity.h"
#include "simple_json.h"

Entity *boss2_spawn(Vector2D position);

void boss2_update(Entity *self);

#endif