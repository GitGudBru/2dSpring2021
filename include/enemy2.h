#ifndef __ENEMY2_H__
#define __ENEMY2_H__

#include "entity.h"
#include "simple_json.h"

Entity *enemy2_spawn(Vector2D position);

void enemy2_update(Entity *self);

//void enemy2_move(Entity *self);

//void enemy2_attack(Entity *self);

//void enemy2_collide(Entity*self, Entity *other);

#endif