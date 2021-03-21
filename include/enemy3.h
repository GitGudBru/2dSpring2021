#ifndef __ENEMY3_H__
#define __ENEMY3_H__

#include "entity.h"
#include "simple_json.h"

Entity *enemy3_spawn(Vector2D position);

void enemy3_update(Entity *self);

//void enemy3_move(Entity *self);

//void enemy3_attack(Entity *self);

//void enemy3_collide(Entity*self, Entity *other);

#endif