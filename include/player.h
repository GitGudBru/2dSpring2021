#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "entity.h"

/**
* @brief spawn a player entity
* @param position the screen position to spawn the player at
* @return NULL on error, or a pointer to a new player entity
*/
//Entity *player_spawn(Vector2D position);
Entity *player_spawn(const char* filename);


/**
* @brief moves an entity/player
* @param self the entity/player to move 
*/
void player_move(Entity *self);

Entity* player_get(); 

void player_save(Entity* self, char* filename);


#endif