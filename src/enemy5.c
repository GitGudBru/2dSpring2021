#include "enemy5.h"
#include "level.h"
#include "player.h"
#include "simple_logger.h"
#include "entity.h"
#include "weapon.h"

//GUNMAN ENEMY

//Entity *playerTarget;
//static int attackTimer = 0;

int  enemy5_damage(Entity *self, int amount, Entity *source);
void enemy5_die(Entity *self);
int  enemy5_touch(Entity *self, Entity *other);
int  enemy5_player_sight_check(Entity *self);
void enemy5_think_hunting(Entity *self);
void enemy5_turn(Entity *self, int dir);
void enemy5_think(Entity *self);

Entity *enemy5_spawn(Vector2D position)
{
	Entity* ent;
	ent = entity_new();
	if (!ent)
	{
		slog("failed to create entity for the player");
		return NULL;
	}
	//playerTarget = target;
	ent->sprite = gf2d_sprite_load_all("images/space_bug.png", 62.5, 53, 7); //ADD ZOMBIE

	vector2d_copy(ent->position, position);
	vector2d_copy(ent->spawn, position);
	ent->frameRate = 0.1;
	ent->frameCount = 7;
	ent->think = enemy5_think;
	ent->update = enemy5_update;
	ent->damage = enemy5_damage;
	ent->health = 1;
	ent->maxHealth = 1;
	ent->projectcool = 0;
	ent->rotation.x = 64;
	ent->rotation.y = 64;
	vector2d_set(ent->flip, 0, 0);
	vector2d_set(ent->forward, -1, 0);
	ent->shape = gf2d_shape_rect(0, 0, 64, 72);
	gf2d_body_set(
		&ent->body,
		"enemy5",
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

void enemy5_think_attacking(Entity *self)
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

	self->think = enemy5_think_hunting;

}


void enemy5_attack(Entity *self)
{
	self->think = enemy5_think_attacking;
}

void enemy5_turn(Entity *self, int dir)
{
	if (dir < 0)
	{
		self->forward.x = 1;
		self->flip.x = 0;
	}
	else if (dir > 0)
	{
		self->forward.x = 0;
		self->flip.x = 1;
	}
}
void enemy5_think_hunting(Entity *self)
{
	Entity *player = player_get();
	float xDistance = abs(self->position.x - player->position.x);

	if ((self->jumpcool) || (self->cooldown))return;
	self->jumpcool = 20;

	if (player->position.x + 40 < self->position.x)
	{
		enemy5_turn(self, -1);
	}
	if (player->position.x + 40 > self->position.x)
	{
		enemy5_turn(self, 1);
	}

	if (vector2d_magnitude_compare(vector2d(self->position.x - player->position.x, self->position.y - player->position.y), 500) > 0) //DETECTION
	{
		slog("lost the player");
		self->think = enemy5_think;// idle think
		return;
	}
	else
	{
		Entity* bomb = bomb_spawn(vector2d(self->position.x + (self->forward.x * -48) + 16, self->position.y), self->forward, PLAYER_LAYER, 0.35);
		level_add_entity(bomb);
		//slog("BOM BOM");
	}
}
void enemy5_think(Entity *self)
{
	if (enemy5_player_sight_check(self))
	{
		self->think = enemy5_think_hunting;
		return;
	}

	if (self->spawn.x - 10 > self->position.x)
	{
		enemy1_turn(self, -1);

	}
	if (self->spawn.x + 10 < self->position.x)
	{
		enemy1_turn(self, -1);

	}
}

void enemy5_update(Entity *self)
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

int  enemy5_damage(Entity *self, int amount, Entity *source)
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
		self->think = enemy5_die;
		//gf2d_actor_set_action(&self->actor,"death1");
	}
	return amount;
}

int enemy5_player_sight_check(Entity *self)
{
	Entity *player = player_get();
	if (!player)return 0;
	if (vector2d_magnitude_compare(vector2d(self->position.x - player->position.x, self->position.y - player->position.y), 500) < 0) //DETECTION
	{
		slog(" sight check found player");
		return 1;
	}
	return 0;
}

void enemy5_die(Entity *self)
{
	slog("im die..");
	level_remove_entity(self);
	entity_free(self);
}
/*eol@eof*/