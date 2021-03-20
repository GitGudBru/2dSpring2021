#include "box.h"
#include "player.h"
#include "level.h"
#include "simple_logger.h"
#include "pickup.h"
#include "entity.h"

void pickup_think(Entity *self);
void pickup_update(Entity *self);
void pickup_activate(Entity *self);



Entity *pickup_new(Vector2D position);

Entity* pickup_spawn(Vector2D position)
{
	Entity* ent;
	ent = entity_new();
	if (!ent)
	{
		slog("failed to create breakable entity");
		return NULL;
	}
	ent->sprite = gf2d_sprite_load_all("images/space_bug_top.png", 128, 128, 16);
	vector2d_copy(ent->position, position);
	ent->frame = 166;
	ent->frameRate = 0.1;
	ent->frameCount = 167;
	ent->rotation.x = 64;
	ent->rotation.y = 64;

	ent->health = 20;
	ent->maxHealth = 20;

	ent->think = pickup_think;
	//ent->draw = breakable_draw;
	ent->update = pickup_update;
	//ent->damage = breakable_damage;
	//ent->die = breakable_die;
	//self->free = level_remove_entity;

	ent->shape = gf2d_shape_rect(0, 0, 40, 40);
	gf2d_body_set(
		&ent->body,
		"breakable",
		1,
		WORLD_LAYER | MONSTER_LAYER | PLAYER_LAYER,
		0,
		0,
		position,
		vector2d(0, 0),
		10,
		1,
		0,
		&ent->shape,
		ent,
		NULL);


	level_add_entity(ent);
	return ent;
}

void pickup_hit(Entity* self)
{
	Shape s;
	int i, count;
	Entity* other;
	Collision* c;
	List* collisionList = NULL;
	s = gf2d_shape_rect(self->position.x + 1, self->position.y - 1, 31, 33);
	collisionList = entity_get_clipped_entities(self, s, PLAYER_LAYER, 0);
	count = gfc_list_get_count(collisionList);
	//slog("hit %i pickup", count);
	for (i = 0; i < count; i++)
	{
		c = (Collision*)gfc_list_get_nth(collisionList, i);
		if (!c)continue;
		if (!c->body)continue;
		if (!c->body->data)continue;
		other = c->body->data;
		//if (other->damage)other->damage(other, 1, self);//TODO: make this based on weapon / player stats
		pickup_activate(other);

		level_remove_entity(self);
		entity_free(self);
	}
	gf2d_collision_list_free(collisionList);
	//level_remove_entity(self);
	//entity_free(self);
}

void pickup_update(Entity *self)
{
	entity_world_snap(self);    // error correction for collision system
	entity_apply_gravity(self);
	if (self->health <= 0) {
		level_remove_entity(self);
		entity_free(self);
	}
}

void pickup_think(Entity *self)
{
	pickup_hit(self);
}

void pickup_activate(Entity *self) //self is player here
{
	slog("PICKED UP OBJECT");
	//self->position.y = 50;
	self->shotgun = 1;
}
/*eol@eof*/