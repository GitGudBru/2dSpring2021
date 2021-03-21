#ifndef __ENEMY1_H__
#define __ENEMY1_H__

#include "entity.h"
#include "simple_json.h"

Entity *enemy1_spawn(Vector2D position);

void enemy1_think(Entity *self);

void enemy1_update(Entity *self);

//void enemy1_move(Entity *self);

//void enemy1_attack(Entity *self);

//void enemy1_collide(Entity*self, Entity *other);
	
#endif