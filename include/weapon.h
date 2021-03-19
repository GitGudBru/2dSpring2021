#ifndef __WEAPON_H__
#define __WEAPON_H__

#include "entity.h"

Entity* handgun_shoot(Vector2D position, Vector2D flip);

Entity* shotgun_shoot(Vector2D position, Vector2D flip);

Entity* grenade_throw(Vector2D position, Vector2D flip);

Entity* knife_attack(Vector2D position, Vector2D flip);

Entity* machinegun_shoot(Vector2D position, Vector2D flip);



#endif