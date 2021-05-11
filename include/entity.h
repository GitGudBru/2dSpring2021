#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gfc_types.h"
#include "gfc_vector.h"
#include "gf2d_sprite.h"
#include "gf2d_collision.h"

typedef struct Entity_s
{
	Bool     _inuse;
	Vector2D position;
	Vector3D    rotation;
	Vector2D velocity;
	Vector2D    flip;
	Vector2D    spawn;
	Sprite   *sprite;
	Sprite	 *sprite2;
	Sprite   *healthicon;
	Shape       shape;
	Body        body;
	float    frame;
	float    frameRate;
	int      frameCount;
	void	 (*activate)(struct Entity_S* self, struct Entity_S* activator);
	void	 (*update)(struct Entity_s *self);
	void	 (*think)(struct Entity_s *self);
	void	 (*draw)(struct Entity_s *self);
	void	 (*free)(struct Entity_s *self);
	int		 (*touch)(struct Entity_S* self, struct Entity_S* other);
	int		 (*damage)(struct Entity_S* self, int amount, struct Entity_S* source);
	void	 (*die)(struct Entity_S* self);
	int	     health;                          
	int		 maxHealth;                     
	int		 cooldown;                        
	int	   	 attack;                          
	int		 count;                           
	float	 jumpcool;
	float	 projectcool;
	float	 throw;
	int		 jumpcount;                      
	int		 grounded;
	int		 bossdir;
	int		 jumping;
	int      shotgun;
	int      machinegun;
	int		 bomb;
	int		 ostrich;
	int		 ufo;
	int		 bot;
	int		 perk1;
	int		 perk2;
	int		 perk3;
	int		 perk4;
	int		 perk5;
	int		 num;
	int		 attacking;
	int		 score;
	char     hitLayer;
	Vector2D forward;
	void     *data;
}Entity;

/**
 *@brief initialize internal entity management system.
 *@param max_entities how many entities to support.
 */
void entity_manager_init(Uint32 max_entities);

/**
* @brief calls update function on all entities
*/
void entity_manager_update_entities();

/**
* @brief call think function for all entities
*/
void entity_manager_think_entities();


/**
* @brief call draw on all entities
*/
void entity_manager_draw_entities();

/**
*@brief free all entities in the system and destory entity manager.
*/
void entity_manager_free();

/**
*@brief allocate an entity, initialize to zero and return a pointer to it.
*@return NULL on error (see logs) or a pointer to an initialized entity.
*/
Entity *entity_new();

/**
*@brief frees provided entity.
*@param ent the entity to free.
*/
void entity_free(Entity *ent);

/**
*@brief draws an entity to the current render frame.
*@param ent the entity to draw.
*/
void entity_draw(Entity *ent);

/**
* @brief adjust position of entity to redice clipping static shapes of the world
* @param self the entity to adjust
*/
void entity_world_snap(Entity* self);

/**
* @brief adjust velocity according to gravity/ test for grounded status
* @param self the entity to adjust
*/
void entity_apply_gravity(Entity* self);

List* entity_get_clipped_entities(Entity* self, Shape s, Uint32 layers, Uint32 team);

void gf2d_entity_free_all();





#endif