#include "boss2.h"
#include "level.h"
#include "player.h"
#include "simple_logger.h"
#include "entity.h"
#include "weapon.h"

//[MESSAGE REDACTED] BOSS

//Entity *playerTarget;
//static int attackTimer = 0;

int  boss2_damage(Entity *self, int amount, Entity *source);
void boss2_die(Entity *self);
int  boss2_touch(Entity *self, Entity *other);
int  boss2_player_sight_check(Entity *self);
void boss2_think_hunting(Entity *self);
void boss2_turn(Entity *self, int dir);
void boss2_think(Entity *self);

Entity *boss2_spawn(Vector2D position)
{
	Entity* ent;
	ent = entity_new();
	if (!ent)
	{
		slog("failed to create entity for the player");
		return NULL;
	}
	//playerTarget = target;
	ent->sprite = gf2d_sprite_load_all("images/boss2/boss_spawn.png", 49.1, 53, 8); //ADD ZOMBIE
	ent->frameCount = 6;
	vector2d_copy(ent->position, position);
	vector2d_copy(ent->spawn, position);
	ent->frameRate = 0.1;
	ent->frameCount = 8;
	ent->think = boss2_think;
	ent->update = boss2_update;
	ent->damage = boss2_damage;
	ent->health = 30;
	ent->maxHealth = 1;
	ent->projectcool = 0;
	ent->rotation.x = 64;
	ent->rotation.y = 64;
	vector2d_set(ent->flip, 0, 0);
	vector2d_set(ent->forward, -1, 0);
	ent->shape = gf2d_shape_rect(0, 0, 64, 72);
	gf2d_body_set(
		&ent->body,
		"boss2",
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

void boss2_think_attacking(Entity *self)
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

	self->think = boss2_think_hunting;

}


void boss2_attack(Entity *self)
{
	self->think = boss2_think_attacking;
}

void boss2_turn(Entity *self, int dir)
{
	if (dir < 0)
	{
		self->forward.x = 1;
		self->flip.x = 1;
	}
	else if (dir > 0)
	{
		self->forward.x = 0;
		self->flip.x = 0;
	}
}
void boss2_think_hunting(Entity *self)
{
	//SPRITE DETECTED PLAYER
	Entity *player = player_get();
	float xDistance = abs(self->position.x - player->position.x);
	if ((self->jumpcool) || (self->cooldown))return;
	self->jumpcool = 20;


	if (vector2d_magnitude_compare(vector2d(self->position.x - player->position.x, self->position.y - player->position.y), 500) > 0) //DETECTION
	{
		//slog("lost the player");
		self->think = boss2_think;// idle think
		return;
	}
	else
	{
		//SPRITE ATTACK
		int random = rand() % 4;

		if (random == 1)
		{
			self->sprite = gf2d_sprite_load_all("images/boss2/boss_red.png", 53, 51, 2); //ADD ZOMBIE
			self->frameCount = 2;
			self->velocity.y = -5;
			//slog("up");
			Entity* bomb = bomb_spawn(vector2d(self->position.x + (self->forward.x * -48) + 16, self->position.y), self->forward, PLAYER_LAYER, 0.35);
			Entity* bomb2 = bomb_spawn(vector2d(self->position.x + (self->forward.x * -48) + 16, self->position.y), self->forward, PLAYER_LAYER, 0.9);
			Entity* bomb3 = bomb_spawn(vector2d(self->position.x + (self->forward.x * -48) + 16, self->position.y), self->forward, PLAYER_LAYER, 0.7);

			level_add_entity(bomb);
			level_add_entity(bomb2);
			level_add_entity(bomb3);
		}
		if (random == 0)
		{
			self->sprite = gf2d_sprite_load_all("images/boss2/boss_red.png", 53, 51, 2); //ADD ZOMBIE
			self->frameCount = 2;
			self->velocity.y = 15;
			//slog("down");
			Entity* bomb = bomb_spawn(vector2d(self->position.x + (self->forward.x * -48) + 16, self->position.y), self->forward, PLAYER_LAYER, 0.35);
			Entity* bomb2 = bomb_spawn(vector2d(self->position.x + (self->forward.x * -48) + 16, self->position.y), self->forward, PLAYER_LAYER, 0.9);
			Entity* bomb3 = bomb_spawn(vector2d(self->position.x + (self->forward.x * -48) + 16, self->position.y), self->forward, PLAYER_LAYER, 0.7);
			
			level_add_entity(bomb);
			level_add_entity(bomb2);
			level_add_entity(bomb3);
		}
		if (random == 2)
		{
			self->sprite = gf2d_sprite_load_all("images/boss2/boss_jump_right.png", 49.1, 55, 7); //ADD ZOMBIE
			self->frameCount = 7;
			self->velocity.x = 2;
			//slog("right");
			Entity* shot1 = boss_attack_shoot(vector2d(self->position.x - 20, self->position.y + 45), 1, 1); //DOWN
			Entity* shot2 = boss_attack_shoot(vector2d(self->position.x, self->position.y + 45), 1, 1); //DOWN
			Entity* shot3 = boss_attack_shoot(vector2d(self->position.x, self->position.y + 32), 1, 0);
			Entity* shot4 = boss_attack_shoot(vector2d(self->position.x, self->position.y + 12), 1, 0);
			Entity* shot5 = boss_attack_shoot(vector2d(self->position.x - 12, self->position.y + 32), 0, 0); //LEFT
			Entity* shot6 = boss_attack_shoot(vector2d(self->position.x - 12, self->position.y + 12), 0, 0); //LEFT

		//	slog("pew pew enemy shoot");
			level_add_entity(shot1);
			level_add_entity(shot2);
			level_add_entity(shot3);
			level_add_entity(shot4);
			level_add_entity(shot5);
			level_add_entity(shot6);
		}
		if (random == 3)
		{
			self->sprite = gf2d_sprite_load_all("images/boss2/boss_jump_left.png", 60.2, 63, 7); //ADD ZOMBIE
			self->frameCount = 7;
			self->velocity.x = -2;
			slog("left");
			Entity* shot1 = boss_attack_shoot(vector2d(self->position.x - 20, self->position.y + 45), 1, 1); //DOWN
			Entity* shot2 = boss_attack_shoot(vector2d(self->position.x, self->position.y + 45), 1, 1); //DOWN
			Entity* shot3 = boss_attack_shoot(vector2d(self->position.x, self->position.y + 32), 1, 0);
			Entity* shot4 = boss_attack_shoot(vector2d(self->position.x, self->position.y + 12), 1, 0);
			Entity* shot5 = boss_attack_shoot(vector2d(self->position.x - 12, self->position.y + 32), 0, 0); //LEFT
			Entity* shot6 = boss_attack_shoot(vector2d(self->position.x - 12, self->position.y + 12), 0, 0); //LEFT

			slog("pew pew enemy shoot");
			level_add_entity(shot1);
			level_add_entity(shot2);
			level_add_entity(shot3);
			level_add_entity(shot4);
			level_add_entity(shot5);
			level_add_entity(shot6);
		}

		if (player->position.x + 40 < self->position.x)
		{
			boss1_turn(self, -1);
		}
		if (player->position.x + 40 > self->position.x)
		{
			boss1_turn(self, 1);
		}
	}
}
void boss2_think(Entity *self)
{
	self->sprite = gf2d_sprite_load_all("images/boss2/boss_idle.png", 49, 50, 6); //ADD ZOMBIE
	self->frameCount = 6;
	//SPRITE BOSS CHILLIN
	if (boss2_player_sight_check(self))
	{
		self->think = boss2_think_hunting;
		return;
	}

	if (self->spawn.x - 10 > self->position.x)
	{
		enemy1_turn(self, -1);
		self->velocity.x = 1;

	}
	if (self->spawn.x + 10 < self->position.x)
	{
		enemy1_turn(self, -1);
		self->velocity.x = -1;
	}
}

void boss2_update(Entity *self)
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

int  boss2_damage(Entity *self, int amount, Entity *source)
{
	Vector2D dir = { 0 };
	//slog("enemy taking %i damage!", amount);	//ADD DAMAGE SPRITE
	self->health -= amount;
	//gf2d_sound_play(self->sound[1],0,0.1,-1,-1);
	vector2d_sub(dir, source->position, self->position);
	vector2d_normalize(&dir);
	vector2d_scale(dir, dir, 3);
	if (self->health <= 0)
	{
		self->health = 0;
		self->think = boss2_die;
	}
	return amount;
}

int boss2_player_sight_check(Entity *self)
{
	Entity *player = player_get();
	if (!player)return 0;
	if (vector2d_magnitude_compare(vector2d(self->position.x - player->position.x, self->position.y - player->position.y), 500) < 0) //DETECTION
	{
		return 1;
	}
	return 0;
}

void boss2_die(Entity *self)
{
	//slog("im die..");
	level_remove_entity(self);
	entity_free(self);
}
/*eol@eof*/