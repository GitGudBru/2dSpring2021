#ifndef __ENEMY5_H__
#define __ENEMY5_H__

#include "entity.h"
#include "simple_json.h"

Entity *enemy5_spawn(Vector2D position);

void enemy5_update(Entity *self);

//void enemy5_move(Entity *self);

//void enemy5_attack(Entity *self);

//void enemy5_collide(Entity*self, Entity *other);

#endif