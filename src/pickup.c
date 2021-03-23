#include "box.h"
#include "player.h"
#include "level.h"
#include "simple_logger.h"
#include "pickup.h"
#include "entity.h"

void pickup_think(Entity *self);
void pickup_update(Entity *self);
void pickup_activate(Entity *self);
void pickup_activate2(Entity *self);
void pickup_activate3(Entity *self);
void pickup_activate4(Entity *self);
void pickup_activate5(Entity *self);




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
	//ent->sprite = gf2d_sprite_load_all("images/space_bug_top.png", 128, 128, 16);
	vector2d_copy(ent->position, position);
	ent->frame = 0;
	ent->frameRate = 0.1;
	ent->frameCount = 1;
	ent->rotation.x = 64;
	ent->rotation.y = 64;

	ent->health = 100;
	ent->maxHealth = 100;
	ent->num = rand() % 5;
	if (ent->num == 0)
	{
		slog("1");
		ent->frameCount = 1;
		ent->sprite = gf2d_sprite_load_all("images/pickup/heavy_pickup.png", 23, 21, 1);
	}
	if (ent->num == 1)
	{
		slog("2");
		ent->frameCount = 1;
		ent->sprite = gf2d_sprite_load_all("images/pickup/shotty_pickup.png", 23, 21, 1);
	}
	if (ent->num == 2)
	{
		slog("3");
		ent->frameCount = 6;
		ent->sprite = gf2d_sprite_load_all("images/pickup/bomb_pickup.png", 26, 22, 6);
		
	}
	if (ent->num == 3)
	{
		slog("4");
		ent->frameCount = 7;
		ent->sprite = gf2d_sprite_load_all("images/pickup/medkit.png", 28.4, 18, 7);
	}
	if (ent->num == 4)
	{
		slog("5");
		ent->frameCount = 16;
		ent->sprite = gf2d_sprite_load_all("images/pickup/banana.png", 21, 21, 16);
	}

	ent->think = pickup_think;
	//ent->draw = breakable_draw;
	ent->update = pickup_update;
	//ent->damage = breakable_damage;
	//ent->die = breakable_die;
	//self->free = level_remove_entity;

	ent->shape = gf2d_shape_rect(0, 0, 40, 40);
	gf2d_body_set(
		&ent->body,
		"pickup",
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
	int i, count, r;
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
		if (self->num == 0)pickup_activate(other);
		if (self->num == 1)pickup_activate2(other);
		if (self->num == 2)pickup_activate3(other);
		if (self->num == 3)pickup_activate4(other);
		if (self->num == 4)pickup_activate5(other);

		//pickup_activate(other);

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
	self->machinegun = 0;
	//slog("PICKED UP SHOTGUN");
	self->shotgun = 1;
	self->score = self->score + 1;

	//r = rand() % 5;
	//r = gfc_random();
	//slog("%r");
	//r = 0;

}
void pickup_activate2(Entity *self) //self is player here
{
	self->shotgun = 0;
	//slog("PICKED UP SPECIAL WEAPON");
	self->machinegun = 1;
	self->score = self->score + 1;


}
void pickup_activate3(Entity *self) //self is player here
{
	
	//slog("PICKED UP BOMB");
	self->bomb = self->bomb + 1;
	self->score = self->score + 1;

}
void pickup_activate4(Entity *self) //self is player here
{
	//slog("PICKED UP MEDKIT");
	self->health = self->health + 1;
	self->score = self->score + 1;


}
void pickup_activate5(Entity *self) //self is player here
{
	//slog("PICKED UP BANANA");
	self->health = 5;
	self->score = self->score + 1;
}

/*eol@eof*/