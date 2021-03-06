#ifndef __WEAPON_H__
#define __WEAPON_H__

#include "entity.h"

Entity* handgun_shoot(Vector2D position, Vector2D flip, int x,char layer);

Entity* shotgun_shoot(Vector2D position, Vector2D flip);

Entity* bomb_spawn(Vector2D position, Vector2D flip, char layer, float throw);

Entity* knife_attack(Vector2D position, Vector2D flip);

Entity* machinegun_shoot(Vector2D position, Vector2D flip);

Entity* boss_attack_shoot(Vector2D position, int flip, int x);

#endif