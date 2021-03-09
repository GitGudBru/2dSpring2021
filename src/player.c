#include "simple_logger.h"
#include "player.h"

void player_update(Entity *self);

static int walking = 0;

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
	return ent;
}

void player_update(Entity *self)
{
	player_move(self);
}

void player_move(Entity *self)
{
	Uint8 *buttons = SDL_GetKeyboardState(NULL);
	//vector2d_set(self->velocity, 0, 1);	//Gravity, make sure to implement isGrounded when- 
											//colliding with floor so that u cant jump if youre not on the ground (NEED TO IMPLEMENT COLLISION FOR FLOOR)
	//Jump right
	if (buttons[SDL_SCANCODE_SPACE] && buttons[SDL_SCANCODE_D]){
		self->position.x += 1;
		self->position.y -= 2;
		self->sprite = gf2d_sprite_load_all("images/jump.png", 35.6, 49, 5);
		self->frameCount = 5;
		return;
	}

	if (buttons[SDL_SCANCODE_SPACE] && buttons[SDL_SCANCODE_A]){
		self->position.x -= 1;
		self->position.y -= 2;
		self->sprite = gf2d_sprite_load_all("images/jump_back.png", 35.6, 49, 5);
		self->frameCount = 5;
		return;
	}

	//Right
	if (buttons[SDL_SCANCODE_D])
	{
		self->position.x += 1;
		self->sprite = gf2d_sprite_load_all("images/walk.png", 39.3, 53, 11);
		//self->sprite = gf2d_sprite_load_all("images/space_bug_top.png", 128, 128, 16);
		walking = 1;
		return;
	}
	//Left
	if (buttons[SDL_SCANCODE_A]){
		self->position.x -= 1;
		self->sprite = gf2d_sprite_load_all("images/walk_back.png", 39.1, 53, 11);
		return;
	}
	if (buttons[SDL_SCANCODE_SPACE]){
		self->position.y -= 2;
		self->sprite = gf2d_sprite_load_all("images/jump.png", 35.6, 49, 5);
		self->frameCount = 5;
		return;
	}
	
}
