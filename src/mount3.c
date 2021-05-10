#include "player.h"
#include "level.h"
#include "simple_logger.h"
#include "gfc_audio.h"
#include "mount3.h"
#include "entity.h"

void mount3_think(Entity *self);
void mount3_update(Entity *self);
void mount3_activate(Entity *self);

Entity *mount3_new(Vector2D position);

Entity *mount3_spawn(Vector2D position)
{
	Entity* ent;
	ent = entity_new();
	if (!ent)
	{
		slog("failed to create mount entity");
		return NULL;
	}

	ent->sprite = gf2d_sprite_load_all("images/bot/bot_empty.png", 58, 49, 1);

	vector2d_copy(ent->position, position);
	ent->frame = 0;
	ent->frameRate = 0.1;
	ent->frameCount = 1;
	ent->rotation.x = 64;
	ent->rotation.y = 64;
	ent->health = 100;
	ent->maxHealth = 100;
	ent->think = mount3_think;
	ent->update = mount3_update;
	ent->shape = gf2d_shape_rect(0, 0, 64, 72);
	gf2d_body_set(
		&ent->body,
		"mount3",
		1,
		WORLD_LAYER | PLAYER_LAYER,
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
	slog("mount3 spawned");

	level_add_entity(ent);
	return ent;
}

void mount3_hit(Entity *self)
{
	{
		Shape s;
		int i, count, r;
		Entity* other;
		Collision* c;
		List* collisionList = NULL;
		s = gf2d_shape_rect(self->position.x + 1, self->position.y - 1, 31, 33);
		collisionList = entity_get_clipped_entities(self, s, PLAYER_LAYER, 0);
		count = gfc_list_get_count(collisionList);
		for (i = 0; i < count; i++)
		{
			c = (Collision*)gfc_list_get_nth(collisionList, i);
			if (!c)continue;
			if (!c->body)continue;
			if (!c->body->data)continue;
			other = c->body->data;
			mount3_activate(other);
			level_remove_entity(self);
			entity_free(self);
		}
		gf2d_collision_list_free(collisionList);

	}
}
void mount3_update(Entity *self)
{
	entity_world_snap(self);    // error correction for collision system
	entity_apply_gravity(self);
	if (self->health <= 0) {
		level_remove_entity(self);
		entity_free(self);
	}
}

void mount3_think(Entity *self)
{
	mount3_hit(self);
}

void mount3_activate(Entity *self)
{
	self->shape = gf2d_shape_rect(16, 5, 30, 50);	//SIZE OF COLLIDER
	gf2d_body_set(
		&self->body,
		"player",
		1,
		PLAYER_LAYER,//WORLD_LAYER,
		0,
		1,
		self->position,
		vector2d(0, 0),
		10,
		1,
		0,
		&self->shape,
		self,
		NULL);

	slog("you are now ostrich");
	self->shotgun = 0;
	self->machinegun = 0;
	self->ostrich = 0;
	self->bot = 1;
	//self->health = 5
}
/*eol@eof*/