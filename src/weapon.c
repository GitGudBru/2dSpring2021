#include "simple_logger.h"

#include "weapon.h"
#include "camera.h"
#include "level.h"


void handgun_update(Entity *self);

void handgun_think(Entity* self);

void handgun_think2(Entity* self);


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





/*eol@eof*/