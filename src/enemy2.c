#include "enemy2.h"
#include "level.h"
#include "player.h"
#include "simple_logger.h"
#include "entity.h"

//ZOMBIE ENEMY

//Entity *playerTarget;
//static int attackTimer = 0;

int  enemy2_damage(Entity *self, int amount, Entity *source);
void enemy2_die(Entity *self);
int  enemy2_touch(Entity *self, Entity *other);
int  enemy2_player_sight_check(Entity *self);
void enemy2_think_hunting(Entity *self);
void enemy2_turn(Entity *self, int dir);
void enemy2_think_patroling(Entity *self);

Entity *enemy2_spawn(Vector2D position)
{
	Entity* ent;
	ent = entity_new();
	if (!ent)
	{
		slog("failed to create entity for the player");
		return NULL;
	}
	//playerTarget = target;
	ent->sprite = gf2d_sprite_load_all("images/bike.png", 62.5, 53, 7); //ADD ZOMBIE

	vector2d_copy(ent->position, position);
	vector2d_copy(ent->spawn, position);
	//ent->frame = 5;
	ent->frameRate = 0.1;
	ent->frameCount = 7;
	ent->think = enemy2_think_hunting;
	ent->update = enemy2_update;
	ent->damage = enemy2_damage;
	ent->health = 3;
	ent->maxHealth = 3;
	ent->projectcool = 0;
	ent->rotation.x = 64;
	ent->rotation.y = 64;
	vector2d_set(ent->flip, 0, 0);
	vector2d_set(ent->forward, -1, 0);
	ent->shape = gf2d_shape_rect(0, 0, 64, 72);
	gf2d_body_set(
		&ent->body,
		"enemy2",
		1,
		MONSTER_LAYER,
		0,
		1,
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

void enemy2_think_attacking(Entity *self)
{

		Shape s;
		int i, count;
		Entity* other;
		Collision* c;
		List* collisionList = NULL;
		s = gf2d_shape_rect(self->position.x + (self->flip.x * -48) + 32, self->position.y, 50, 40);
		collisionList = entity_get_clipped_entities(self, s, PLAYER_LAYER, 0);
		count = gfc_list_get_count(collisionList);
		slog("hit %i player", count);
		for (i = 0; i < count; i++)
		{
			c = (Collision*)gfc_list_get_nth(collisionList, i);
			if (!c)continue;
			if (!c->body)continue;
			if (!c->body->data)continue;
			other = c->body->data;
			if (other->damage)other->damage(other, 1, self);
		}
		gf2d_collision_list_free(collisionList);

		self->think = enemy2_think_hunting;

		//self->projectcool = 15;

}


void enemy2_attack(Entity *self)
{
	self->think = enemy2_think_attacking;
}


void enemy2_think_hunting(Entity *self)
{
	Entity *player = player_get();
	float xDistance = abs(self->position.x - player->position.x);
	//slog("think hunting");

	if ((self->jumpcool) || (self->cooldown))return;
	self->jumpcool = 20;
	self->velocity.x = -2;
	if (player->position.x + 80 < self->position.x)
	{
		self->velocity.x = - 2;
	}
		if (player->position.x - self->position.x >= -10)
	{
		slog("oof");
		player->health = player->health - 1;

	}
	//enemy2_attack(self);
}

void enemy2_update(Entity *self)
{
	if (self->jumpcool > 0) self->jumpcool -= 0.2;
	else self->jumpcool = 0;
	if (self->projectcool > 0) self->projectcool -= 0.2;
	else self->projectcool = 0;
	//world clipping
	if (self->cooldown > 0) self->cooldown--;
	if (self->cooldown < 0)self->cooldown = 0;

	entity_apply_gravity(self);
	entity_world_snap(self);
}

int  enemy2_damage(Entity *self, int amount, Entity *source)
{
	Vector2D dir = { 0 };
	slog("enemy taking %i damage!", amount);
	self->health -= amount;
	//gf2d_sound_play(self->sound[1],0,0.1,-1,-1);
	vector2d_sub(dir, source->position, self->position);
	vector2d_normalize(&dir);
	vector2d_scale(dir, dir, 3);
	if (self->health <= 0)
	{
		self->health = 0;
		self->think = enemy2_die;
		//gf2d_actor_set_action(&self->actor,"death1");
	}
	return amount;
}

void enemy2_die(Entity *self)
{
	slog("im die..");
	level_remove_entity(self);
	entity_free(self);
}
/*eol@eof*/