#include "box.h"
#include "player.h"
#include "level.h"
#include "simple_logger.h"
#include "pickup.h"
#include "entity.h"

void breakable_draw(Entity *self);
void breakable_think(Entity *self);
void breakable_update(Entity *self);
void breakable_activate();
int  breakable_touch(Entity *self, Entity *other);
int  breakable_damage(Entity *self, int amount, Entity *source);
void breakable_die(Entity *self);


Entity *breakable_new(Vector2D position);

Entity* breakable_spawn(Vector2D position)
{
	Entity* ent;
	ent = entity_new();
	if (!ent)
	{
		slog("failed to create breakable entity");
		return NULL;
	}
	ent->sprite = gf2d_sprite_load_all("images/pickup/box.png", 33, 29, 1);
	vector2d_copy(ent->position, position);
	ent->frame = 0;
	ent->frameRate = 0.1;
	ent->frameCount = 1;
	ent->rotation.x = 64;
	ent->rotation.y = 64;

	ent->health = 20;
	ent->maxHealth = 20;

	ent->think = breakable_think;
	//ent->draw = breakable_draw;
	ent->update = breakable_update;
	ent->touch = breakable_touch;
	ent->damage = breakable_damage;
	ent->die = breakable_die;
	ent->activate = breakable_activate;
	//self->free = level_remove_entity;

	ent->shape = gf2d_shape_rect(0, 0, 40, 40);
	gf2d_body_set(
		&ent->body,
		"breakable",
		1,
		WORLD_LAYER | MONSTER_LAYER,
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

void breakable_draw(Entity *self)
{

}

void breakable_activate()//, Entity *activator)
{
}

void breakable_think(Entity *self)
{
}

void breakable_update(Entity *self)
{
	entity_world_snap(self);    // error correction for collision system
	entity_apply_gravity(self);
	if (self->health <= 0) {
		level_remove_entity(self);
		entity_free(self);
		breakable_die(self);
	}
}

int  breakable_touch(Entity *self, Entity *other)
{
	return 0;
}

int  breakable_damage(Entity *self, int amount, Entity *source)
{
	slog("CRUNCH");
	self->health -= amount;
}

void breakable_die(Entity *self)
{
	slog("DESSSSSS");
	Entity* pickup = pickup_spawn(vector2d(self->position.x, self->position.y));
}

/*eol@eof*/