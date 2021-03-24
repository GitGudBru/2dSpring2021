#ifndef __ENEMY3_H__
#define __ENEMY3_H__

#include "entity.h"
#include "simple_json.h"

Entity *enemy3_spawn(Vector2D position);

void enemy3_update(Entity *self);


#endif