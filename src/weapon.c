#include "simple_logger.h"

#include "weapon.h"
#include "camera.h"
#include "level.h"


void handgun_update(Entity *self);

void handgun_think(Entity* self);

void handgun_think2(Entity* self);

void shotgun_update(Entity *self);

void shotgun_think(Entity* self);

void shotgun_think2(Entity* self);

void bomb_update(Entity* self);

void bomb_think(Entity* self);


Entity *handgun_shoot(Vector2D position, Vector2D flip, int x)
{
	Entity *ent;
	ent = entity_new();
	if (!ent)
	{
		slog("failed to create entity for the dagger");
		return NULL;
	}
	ent->sprite = gf2d_sprite_load_all("images/handgun/gun_bullet.png", 15, 10, 1);
	vector2d_copy(ent->position, position);
	//ent->frameAnimStart = 0;
	ent->frame = 0;
	ent->frameRate = 0.1;
	ent->frameCount = 1;
	ent->update = handgun_update;
	if (x == 0)ent->think = handgun_think;
	else
	{
		ent->sprite = gf2d_sprite_load_all("images/handgun/gun_bullet_down.png", 10, 15, 1);
		ent->think = handgun_think2;
	}
	ent->flip = flip;
	ent->rotation.x = 64;
	ent->rotation.y = 64;
	ent->shape = gf2d_shape_rect(16, 0, 30, 16);
	gf2d_body_set(
		&ent->body,
		"handgun",
		1,
		OBJECT_LAYER,
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


	return ent;
}

void dagger_melee(Entity* self)
{
	Shape s;
	int i, count;
	Entity* other;
	Collision* c;
	List* collisionList = NULL;
	s = gf2d_body_to_shape(&self->body);
	gf2d_shape_move(&s, vector2d(0.1, 0));
	collisionList = entity_get_clipped_entities(self, s, MONSTER_LAYER, 0);
	count = gfc_list_get_count(collisionList);
	//slog("hit %i targets", count);
	for (i = 0; i < count; i++)
	{
		c = (Collision*)gfc_list_get_nth(collisionList, i);
		if (!c)continue;
		if (!c->body)continue;
		if (!c->body->data)continue;
		other = c->body->data;
		if (other->damage)other->damage(other, 20, self);
	}
	gf2d_collision_list_free(collisionList);
}

void handgun_update(Entity *self)
{

	Vector2D camera;
	Vector2D cameraSize;

	if (!self)return;

	// apply dampening on velocity
	vector2d_scale(self->velocity, self->velocity, 0.75);
	if (vector2d_magnitude_squared(self->velocity) < 2)
	{
		vector2d_clear(self->velocity);
	}

	if (self->jumpcool > 0) self->jumpcool -= 0.2;
	else self->jumpcool = 0;

	entity_world_snap(self);    // error correction for collision system
}

void handgun_think(Entity* self)
{

	const Uint8* keys;

	Vector2D aimdir, camera, thrust;
	float angle;
	int mx, my;
	if (!self)return;
	keys = SDL_GetKeyboardState(NULL);
	SDL_GetMouseState(&mx, &my);
	if (self->flip.x == 0) {
		vector2d_scale(thrust, vector2d(-1, 0), 1.85);
	}
	else {
		vector2d_scale(thrust, vector2d(1, 0), 1.85);
	}
	vector2d_add(self->velocity, self->velocity, thrust);

	Shape s;
	int i, count;
	Collision* c;
	List* collisionList;
	CollisionFilter filter = {
		1,
		WORLD_LAYER,
		0,
		0,
		&self->body
	};

	if (!self)return 0;
	s = gf2d_body_to_shape(&self->body);
	gf2d_shape_move(&s, vector2d(0.1, 0));

	collisionList = gf2d_collision_check_space_shape(level_get_space(), s, filter);
	if (collisionList != NULL)
	{
		count = gfc_list_get_count(collisionList);
		for (i = 0; i < count; i++)
		{
			c = (Collision*)gfc_list_get_nth(collisionList, i);
			if (!c)continue;
			if (!c->shape)continue;
			gf2d_shape_draw(*c->shape, gfc_color(255, 255, 0, 255), camera_get_offset());
			level_remove_entity(self);
			entity_free(self);
		}
		gf2d_collision_list_free(collisionList);

	}
	dagger_melee(self);

	if (self->jumpcool >= 30) {
		level_remove_entity(self);
		entity_free(self);
	}
}

void handgun_think2(Entity* self)
{

	const Uint8* keys;

	Vector2D aimdir, camera, thrust;
	float angle;
	int mx, my;
	if (!self)return;
	keys = SDL_GetKeyboardState(NULL);
	SDL_GetMouseState(&mx, &my);
	self->rotation.x = 90;
	self->rotation.y = 90;
	vector2d_scale(thrust, vector2d(0, 1), 1.85);
	
	vector2d_add(self->velocity, self->velocity, thrust);

	Shape s;
	int i, count;
	Collision* c;
	List* collisionList;
	CollisionFilter filter = {
		1,
		WORLD_LAYER,
		0,
		0,
		&self->body
	};

	if (!self)return 0;
	s = gf2d_body_to_shape(&self->body);
	gf2d_shape_move(&s, vector2d(0.1, 0));

	collisionList = gf2d_collision_check_space_shape(level_get_space(), s, filter);
	if (collisionList != NULL)
	{
		count = gfc_list_get_count(collisionList);
		for (i = 0; i < count; i++)
		{
			c = (Collision*)gfc_list_get_nth(collisionList, i);
			if (!c)continue;
			if (!c->shape)continue;
			gf2d_shape_draw(*c->shape, gfc_color(255, 255, 0, 255), camera_get_offset());
			level_remove_entity(self);
			entity_free(self);
		}
		gf2d_collision_list_free(collisionList);

	}
	dagger_melee(self);

	if (self->jumpcool >= 30) {
		level_remove_entity(self);
		entity_free(self);
	}
}


Entity *shotgun_shoot(Vector2D position, Vector2D flip, int x)
{
	Entity* ent;
	ent = entity_new();
	if (!ent)
	{
		slog("failed to create entity for the shotgun pallet");
		return NULL;
	}

	ent->sprite = gf2d_sprite_load_all("images/shotgun/pallet.png", 89.5, 63, 6);
	vector2d_copy(ent->position, position);
	//ent->frameAnimStart = 5;
	ent->frame = 5;
	ent->frameRate = 0.1;
	ent->frameCount = 6;
	ent->update = shotgun_update;
	if (x == 0)ent->think = shotgun_think;
	else
	{
		ent->sprite = gf2d_sprite_load_all("images/shotgun/pallet_down.png", 89.3, 95, 6); //CHANGE ALL PALLET_DOWN TO CORRECT SPRITE
		ent->think = shotgun_think;
	}
	ent->flip = flip;
	ent->rotation.x = 64;
	ent->rotation.y = 64;
	ent->shape = gf2d_shape_rect(0, 0, 66, 66);
	gf2d_body_set(
		&ent->body,
		"shotgun pallet",
		1,
		OBJECT_LAYER,
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


	return ent;
}

void shotgun_hit(Entity* self)
{
	Shape s;
	int i, count;
	Entity* other;
	Collision* c;
	List* collisionList = NULL;




	s = gf2d_shape_rect(self->position.x + (self->flip.x * -48) - 16, self->position.y, 64, 64);
	collisionList = entity_get_clipped_entities(self, s, MONSTER_LAYER, 0);
	count = gfc_list_get_count(collisionList);
	slog("hit %i targets", count);
	for (i = 0; i < count; i++)
	{
		c = (Collision*)gfc_list_get_nth(collisionList, i);
		if (!c)continue;
		if (!c->body)continue;
		if (!c->body->data)continue;
		other = c->body->data;
		if (other->damage)other->damage(other, 20, self);//TODO: make this based on weapon / player stats
	}
	gf2d_collision_list_free(collisionList);
}

void shotgun_update(Entity *self)
{
	Vector2D camera;
	Vector2D cameraSize;

	if (!self)return;
	// apply dampening on velocity
	vector2d_scale(self->velocity, self->velocity, 0.75);
	if (vector2d_magnitude_squared(self->velocity) < 2)
	{
		vector2d_clear(self->velocity);
	}
	entity_world_snap(self);    // error correction for collision system
	self->jumpcool += 0.2;

}

void shotgun_think(Entity* self)
{
	const Uint8* keys;

	Vector2D aimdir, camera, thrust;
	float angle;
	int mx, my;
	if (!self)return;
	keys = SDL_GetKeyboardState(NULL);
	SDL_GetMouseState(&mx, &my);
	if (self->flip.x == 0) {
		self->sprite = gf2d_sprite_load_all("images/shotgun/pallet_flip.png", 89.5, 63, 6);
	}
	Shape s;
	int i, count;
	Collision* c;
	List* collisionList;
	CollisionFilter filter = {
		1,
		WORLD_LAYER,
		0,
		0,
		&self->body
	};

	if (!self)return 0;
	s = gf2d_body_to_shape(&self->body);
	gf2d_shape_move(&s, vector2d(0.1, 0));

	collisionList = gf2d_collision_check_space_shape(level_get_space(), s, filter);
	if (collisionList != NULL)
	{
		count = gfc_list_get_count(collisionList);
		for (i = 0; i < count; i++)
		{
			c = (Collision*)gfc_list_get_nth(collisionList, i);
			if (!c)continue;
			if (!c->shape)continue;
			gf2d_shape_draw(*c->shape, gfc_color(255, 255, 0, 255), camera_get_offset());
			//level_remove_entity(self);
			//entity_free(self);
		}
		gf2d_collision_list_free(collisionList);

	}


	if (self->jumpcool >= 15) {
		shotgun_hit(self);



		level_remove_entity(self);
		entity_free(self);

	}

}


Entity* bomb_spawn(Vector2D position, Vector2D flip)
{
	Entity* ent;
	ent = entity_new();
	if (!ent)
	{
		slog("failed to create entity for the dagger");
		return NULL;
	}
	//ent->sprite = gf2d_sprite_load_all("images/ed210_top.png",128,128,16);
	ent->sprite = gf2d_sprite_load_all("images/bomb2/bomb2.png", 22.5, 24, 6);
	//ent->laser = gf2d_sprite_load_all("images/Laser_sheet.png", 300, 100, 1);
	vector2d_copy(ent->position, position);
	//ent->frame = 5;
	ent->frameRate = 0.1;
	ent->frameCount = 6;
	ent->update = bomb_update;
	ent->think = bomb_think;
	ent->flip = flip;
	ent->rotation.x = 64;
	ent->rotation.y = 64;
	ent->shape = gf2d_shape_rect(0, 0, 16, 16);
	gf2d_body_set(
		&ent->body,
		"bomb",
		1,
		OBJECT_LAYER,
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


	return ent;
}

void bomb_melee(Entity* self)
{
	/*
	Shape s;
	int i, count;
	Entity* other;
	Collision* c;
	List* collisionList = NULL;
	s = gf2d_shape_rect(self->position.x + (self->flip.x * -48) - 16, self->position.y, 64, 64);
	collisionList = entity_get_clipped_entities(self, s, MONSTER_LAYER, 0);
	count = gfc_list_get_count(collisionList);
	//slog("hit %i targets", count);
	for (i = 0; i < count; i++)
	{
		c = (Collision*)gfc_list_get_nth(collisionList, i);
		if (!c)continue;
		if (!c->body)continue;
		if (!c->body->data)continue;
		other = c->body->data;
		if (other->damage)other->damage(other, 40, self);
	}
	gf2d_collision_list_free(collisionList);
	*/
	Shape s;
	int i, count;
	Entity* other;
	Collision* c;
	List* collisionList = NULL;
	s = gf2d_body_to_shape(&self->body);
	gf2d_shape_move(&s, vector2d(0.1, 0));
	collisionList = entity_get_clipped_entities(self, s, MONSTER_LAYER, 0);
	count = gfc_list_get_count(collisionList);
	//slog("hit %i targets", count);
	for (i = 0; i < count; i++)
	{
		c = (Collision*)gfc_list_get_nth(collisionList, i);
		if (!c)continue;
		if (!c->body)continue;
		if (!c->body->data)continue;
		other = c->body->data;
		if (other->damage)other->damage(other, 1, self);//TODO: make this based on weapon / player stats
	}
	gf2d_collision_list_free(collisionList);
}

void bomb_update(Entity* self)
{
	Vector2D camera;
	Vector2D cameraSize;

	if (!self)return;
	vector2d_scale(self->velocity, self->velocity, 0.75);
	if (vector2d_magnitude_squared(self->velocity) < 2)
	{
		vector2d_clear(self->velocity);
	}
	entity_world_snap(self);    // error correction for collision system
	self->jumpcool += 0.2;
}

void bomb_think(Entity* self)
{

	const Uint8* keys;

	Vector2D aimdir, camera, thrust;
	float angle;
	int mx, my;
	if (!self)return;
	keys = SDL_GetKeyboardState(NULL);
	SDL_GetMouseState(&mx, &my);

	vector2d_scale(thrust, vector2d(0, -1), -0.75*abs(self->jumpcool) + 2);
	vector2d_add(self->velocity, self->velocity, thrust);
	if (self->flip.x == 1) {
		vector2d_scale(thrust, vector2d(-1, 0), 1.85);
	}
	else {
		vector2d_scale(thrust, vector2d(1, 0), 1.85);
	}
	vector2d_add(self->velocity, self->velocity, thrust);

	Shape s;
	int i, count;
	Collision* c;
	List* collisionList;
	CollisionFilter filter = {
		1,
		WORLD_LAYER,
		0,
		0,
		&self->body
	};

	if (!self)return 0;
	s = gf2d_body_to_shape(&self->body);
	gf2d_shape_move(&s, vector2d(0.1, 0));

	collisionList = gf2d_collision_check_space_shape(level_get_space(), s, filter);
	if (collisionList != NULL)
	{
		count = gfc_list_get_count(collisionList);
		for (i = 0; i < count; i++)
		{
			c = (Collision*)gfc_list_get_nth(collisionList, i);
			if (!c)continue;
			if (!c->shape)continue;
			gf2d_shape_draw(*c->shape, gfc_color(255, 255, 0, 255), camera_get_offset());
			level_remove_entity(self);
			entity_free(self);
		}
		gf2d_collision_list_free(collisionList);

	}
	bomb_melee(self);

	if (self->jumpcool >= 30) {
		level_remove_entity(self);
		entity_free(self);
	}

}




/*eol@eof*/