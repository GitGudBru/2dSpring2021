#include "player.h"
#include "level.h"
#include "simple_logger.h"
#include "gfc_audio.h"
#include "mount2.h"
#include "entity.h"

void mount2_think(Entity *self);
void mount2_update(Entity *self);
void mount2_activate(Entity *self);

Entity *mount2_new(Vector2D position);

Entity *mount2_spawn(Vector2D position)
{
	Entity* ent;
	ent = entity_new();
	if (!ent)
	{
		slog("failed to create mount entity");
		return NULL;
	}

	ent->sprite = gf2d_sprite_load_all("images/ufo/ufo_idle.png", 50, 40, 1);

	vector2d_copy(ent->position, position);
	ent->frame = 0;
	ent->frameRate = 0.1;
	ent->frameCount = 1;
	ent->rotation.x = 64;
	ent->rotation.y = 64;
	ent->health = 100;
	ent->maxHealth = 100;
	ent->think = mount2_think;
	ent->update = mount2_update;
	ent->shape = gf2d_shape_rect(0, 0, 64, 30);
	gf2d_body_set(
		&ent->body,
		"mount2",
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
	slog("mount2 spawned");

	level_add_entity(ent);
	return ent;
}

void mount2_hit(Entity *self)
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
			mount2_activate(other);
			level_remove_entity(self);
			entity_free(self);
		}
		gf2d_collision_list_free(collisionList);

	}
}
void mount2_update(Entity *self)
{
	entity_world_snap(self);    // error correction for collision system
	//entity_apply_gravity(self);
	if (self->health <= 0) {
		level_remove_entity(self);
		entity_free(self);
	}
}

void mount2_think(Entity *self)
{
	mount2_hit(self);
}

void mount2_activate(Entity *self)
{

	slog("you are now ufo");
	self->shotgun = 0;
	self->machinegun = 0;
	self->ostrich = 0;
	self->ufo = 1;
	//self->health = 5
}
/*eol@eof*/