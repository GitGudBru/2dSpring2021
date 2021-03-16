#include "simple_logger.h"
#include "player.h"
#include "camera.h"

static Entity* _player = NULL;

void player_update(Entity *self);
/*
void player_attack_light(Entity* self);
void player_attack_light2(Entity* self);
void player_attack_heavy(Entity* self);
void player_attack_slide(Entity* self);
*/

static int forward = 1;


Entity* player_get()
{
	return _player;
}


Entity *player_spawn(Vector2D position)
{
	Entity *ent;
	ent = entity_new();
	if (!ent)
	{
		slog("failed to create entity for the player");
		return NULL;
	}
	ent->sprite = gf2d_sprite_load_all("images/idle.png", 37.5, 49, 4);
	vector2d_copy(ent->position, position);
	ent->frameRate = 0.1;
	ent->frameCount = 4;
	ent->update = player_update;

	ent->shape = gf2d_shape_rect(16, 5, 30, 40);
	gf2d_body_set(
		&ent->body,
		"player",
		1,
		WORLD_LAYER,
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

void player_update(Entity *self)
{
	Vector2D camera;
	Vector2D cameraSize;

	if (!self)return;
	cameraSize = camera_get_dimensions();
	camera.x = (self->position.x + 200) - (cameraSize.x * 0.5);
	camera.y = (self->position.y + 64) - (cameraSize.y * 0.5);
	camera_set_position(camera);

	player_move(self);

	entity_world_snap(self);    // error correction for collision system
	entity_apply_gravity(self);
	self->velocity.y += .5;


}

void player_move(Entity *self)
{
	Uint8 *buttons = SDL_GetKeyboardState(NULL);
	//vector2d_set(self->velocity, 0, 1);	//Gravity, make sure to implement isGrounded when- 
											//colliding with floor so that u cant jump if youre not on the ground (NEED TO IMPLEMENT COLLISION FOR FLOOR)

	//Jump right
	if (buttons[SDL_SCANCODE_SPACE] && buttons[SDL_SCANCODE_D]) //&& (self->grounded))
	{
		self->position.x += 1;
		self->position.y -= 2;
		self->sprite = gf2d_sprite_load_all("images/jump.png", 35.6, 49, 5);
		self->frameCount = 5; 
		forward = 1;
		return;
	}

	else if (buttons[SDL_SCANCODE_SPACE] && buttons[SDL_SCANCODE_A]) //&& (self->grounded))
	{
		self->position.x -= 1;
		self->position.y -= 2;
		self->sprite = gf2d_sprite_load_all("images/jump_back.png", 35.6, 49, 5);
		self->frameCount = 5;
		forward = 0;
		return;
	}

	//Right
	else if (buttons[SDL_SCANCODE_D])
	{
		self->position.x += 1;
		self->sprite = gf2d_sprite_load_all("images/walk.png", 39.3, 53, 11);
		//self->sprite = gf2d_sprite_load_all("images/space_bug_top.png", 128, 128, 16);
		forward = 1;
		return;
	}
	
	//Left
	else if (buttons[SDL_SCANCODE_A]){
		self->position.x -= 1;
		//self->velocity.x = -1;
		self->sprite = gf2d_sprite_load_all("images/walk_back.png", 39.1, 53, 11);
		forward = 0;
		return;
	}
	else if (buttons[SDL_SCANCODE_SPACE]) //&& (self->grounded)){
	{
		self->position.y -= 2;
		self->sprite = gf2d_sprite_load_all("images/jump.png", 35.6, 49, 5);
		self->frameCount = 5;
		return;
	}
	else {
		if (forward)
		{
			self->sprite = gf2d_sprite_load_all("images/idle.png", 37.5, 49, 4);
			self->frameCount = 4;
			return;
		}
		else if (!forward)
		{
			self->sprite = gf2d_sprite_load_all("images/idleback.png", 36.5, 49, 4);
			self->frameCount = 4;
			return;
		}
	}
	
}
