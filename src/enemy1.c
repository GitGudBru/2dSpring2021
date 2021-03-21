#include "enemy1.h"
#include "level.h"
#include "player.h"
#include "simple_logger.h"
#include "entity.h"

//ZOMBIE ENEMY

//Entity *playerTarget;
//static int attackTimer = 0;

int  enemy1_damage(Entity *self, int amount, Entity *source);
void enemy1_die(Entity *self);
int  enemy1_touch(Entity *self, Entity *other);
int  enemy1_player_sight_check(Entity *self);
void enemy1_think_hunting(Entity *self);
void enemy1_turn(Entity *self, int dir);
void enemy1_think_patroling(Entity *self);

Entity *enemy1_spawn(Vector2D position)
{
	Entity* ent;
	ent = entity_new();
	if (!ent)
	{
		slog("failed to create entity for the player");
		return NULL;
	}
	//playerTarget = target;
	ent->sprite = gf2d_sprite_load_all("images/ed210_top.png",128,128,16); //ADD ZOMBIE

	vector2d_copy(ent->position, position);
	vector2d_copy(ent->spawn, position);
	//ent->frame = 5;
	ent->frameRate = 0.1;
	ent->frameCount = 7;
	ent->think = enemy1_think_patroling;
	ent->update = enemy1_update;
	ent->damage = enemy1_damage;
	ent->health = 3;
	ent->maxHealth = 20;
	ent->projectcool = 0;
	ent->rotation.x = 64;
	ent->rotation.y = 64;
	vector2d_set(ent->flip, 0, 0);
	vector2d_set(ent->forward, -1, 0);
	ent->shape = gf2d_shape_rect(0, 0, 64, 72);
	gf2d_body_set(
		&ent->body,
		"enemy1",
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

void enemy1_think_attacking(Entity *self)
{

	//self->frameAnimStart = 10;
	if (self->frame < 10) {
		self->frame = 10;
	}
	//i++;
	self->frameCount = 16;
	//slog("gained sight of the player");
	if (self->frame >= 14) {



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
			if (other->damage)other->damage(other, 3, self);//TODO: make this based on weapon / player stats
		}
		gf2d_collision_list_free(collisionList);

		self->think = enemy1_think_hunting;

		self->projectcool = 15;
	}

}


void enemy1_attack(Entity *self)
{
	//slog("attacking player");
	//gf2d_actor_set_action(&self->actor,"attack1");
	//self->cooldown = gf2d_actor_get_frames_remaining(&self->actor);
	self->think = enemy1_think_attacking;
	//gf2d_sound_play(self->sound[1],0,1,-1,-1);
}

void enemy1_think_patroling(Entity *self)
{
	//self->frameAnimStart = 0;

	//i++;
	self->frameCount = 2;
	//slog("think patrolling");
	if (enemy1_player_sight_check(self))
	{
		//slog("player found");
		self->think = enemy1_think_hunting;
		return;
	}
	/*if ((!entity_platform_end_check(self))||entity_wall_check(self, vector2d(3 *self->facing.x,0)))
	{
	skeleton_turn(self,self->facing.x * -1);
	}*/


	//Entity* player = player_get();

	if (self->spawn.x - 10 > self->position.x)
	{
		enemy1_turn(self, -1);

	}
	if (self->spawn.x + 10 < self->position.x)
	{
		enemy1_turn(self, -1);

	}
}

void enemy1_turn(Entity *self, int dir)
{
	if (dir < 0)
	{
		self->forward.x = 0;
		self->flip.x = 0;
	}
	else if (dir > 0)
	{
		self->forward.x = 1;
		self->flip.x = 1;
	}
}

void enemy1_think_hunting(Entity *self)
{
	Entity *player = player_get();
	float xDistance = abs(self->position.x - player->position.x);
	//slog("think hunting");
	//self->frameAnimStart = 0;
	if (self->frame > 2) {
		self->frame = 0;
	}
	//i++;
	self->frameCount = 2;

	if ((self->jumpcool) || (self->cooldown))return;

	if (vector2d_magnitude_compare(vector2d(self->position.x - player->position.x, self->position.y - player->position.y), 150) > 0)
	{
		slog("lost the player");
		self->think = enemy1_think_patroling;// idle think
		return;
	}
	self->jumpcool = 20;
	self->velocity.x = -2;
	if (player->position.x + 40 < self->position.x)
	{
		enemy1_turn(self, 1);
		//slog("tuuurn");
		self->velocity.x = -0.5;
	}
	if (player->position.x + 40 > self->position.x)
	{
		enemy1_turn(self, 1);
		//slog("tuuurn2");
		self->velocity.x = 0.5;
	}
	enemy1_attack(self);
}

void enemy1_think(Entity *self)
{
	if (enemy1_player_sight_check(self))
	{
		self->think = enemy1_think_hunting;
	}
}

void enemy1_update(Entity *self)
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

//	self->velocity.y += .5;
}

int  enemy1_touch(Entity *self, Entity *other)
{
	slog("enemy touch called");
	if (!other)return 0;
	return 0;
}

int  enemy1_damage(Entity *self, int amount, Entity *source)
{
	Vector2D dir = { 0 };
	slog("enemy taking %i damage!", amount);
	self->health -= amount;
	//gf2d_sound_play(self->sound[1],0,0.1,-1,-1);
	vector2d_sub(dir, source->position, self->position);
	vector2d_normalize(&dir);
	vector2d_scale(dir, dir, 3);
	//particle_spray(self->position, dir,gf2d_color8(240,0,0,255), 100);
	if (self->health <= 0)
	{
		self->health = 0;
		self->think = enemy1_die;
		//gf2d_actor_set_action(&self->actor,"death1");
	}
	else
	{
		//gf2d_actor_set_action(&self->actor,"pain1");
		//self->cooldown = gf2d_actor_get_frames_remaining(&self->actor);
	}
	return amount;//todo factor in shields}
}

int enemy1_player_sight_check(Entity *self)
{
	Entity *player = player_get();
	//slog("getting player");
	if (!player)return 0;
	//slog("Health : %0.0f", vector2d_magnitude_compare(vector2d(self->position.x - player->position.x, self->position.y - player->position.y), 100));
	if (vector2d_magnitude_compare(vector2d(self->position.x - player->position.x, self->position.y - player->position.y), 100) < 0)
	{
		slog(" sight check found player");
		//gf2d_sound_play(self->sound[0],0,1,-1,-1);
		return 1;
	}
	return 0;
}

void enemy1_die(Entity *self)
{
	slog("im die..");
	level_remove_entity(self);
	entity_free(self);
}
/*eol@eof*/