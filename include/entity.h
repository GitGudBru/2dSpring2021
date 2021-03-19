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
	//Vector2D    facing;
	Sprite   *sprite;
	Sprite	 *sprite2;
	//Sprite*     laser;
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
	int		 (*touch)(struct Entity_S* self, struct Entity_S* other);/**<when this entity touches another entity*/
	float	 health;                           /**<health of entity*/
	int		 maxHealth;                        /**<maximum health of entity*/
	int		 cooldown;                         /**<useful for timing cooldowns*/
	int	   	 attack;                           /**<which attack is being performed*/
	int		 count;                            /**<useful for counting things like ammo count or health ammount*/
	float	 jumpcool;
	float	 projectcool;
	int		 jumpcount;                        /**<used for multijump*/
	int		 grounded;
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


#endif