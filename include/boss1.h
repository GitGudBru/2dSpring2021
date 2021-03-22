#ifndef __BOSS1_H__
#define __BOSS1_H__

#include "entity.h"
#include "simple_json.h"

Entity *boss1_spawn(Vector2D position);

void boss1_update(Entity *self);

#endif