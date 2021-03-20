#include "simple_logger.h"
#include "player.h"
#include "camera.h"
#include "weapon.h"
#include "level.h"

static Entity* _player = NULL;

void player_update(Entity *self);
void player_think(Entity* self);
int  player_damage(Entity* self, int amount, Entity* source);

//static int forward = 1;


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
	ent->think = player_think;
	ent->damage = player_damage;
	ent->health = 2;
	ent->maxHealth = 100;
	ent->forward.x = 1;
	ent->shotgun = 0;
	ent->machinegun = 0;


	ent->shape = gf2d_shape_rect(16, 5, 30, 40);	//SIZE OF COLLIDER
	gf2d_body_set(
		&ent->body,
		"player",
		1,
		PLAYER_LAYER,//WORLD_LAYER,
		0,
		1,
		position,
		//vector2d(ent->position.x, ent->position.y),
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

	if (self->jumpcool > 0) self->jumpcool -= 0.2;
	else self->jumpcool = 0;

	if (self->projectcool > 0) self->projectcool -= 0.2;
	else self->projectcool = 0;

	//player_move(self);

	entity_world_snap(self);    // error correction for collision system
	entity_apply_gravity(self);

	self->velocity.y += .5; //GRAVITY

	if (self->health <= 0) {
		//slog("u ded");
	}
}

void player_think(Entity *self)
{
	Uint8 *buttons = SDL_GetKeyboardState(NULL);
	//vector2d_set(self->velocity, 0, 1);	//Gravity, make sure to implement isGrounded when- 
	//colliding with floor so that u cant jump if youre not on the ground (NEED TO IMPLEMENT COLLISION FOR FLOOR)

	if (buttons[SDL_SCANCODE_SPACE] && (self->jumpcool <= 0)) 
	{
		self->velocity.y -= 14;
		//self->sprite = gf2d_sprite_load_all("images/handgun/jump.png", 35.1, 52, 11);
		//self->sprite2 = gf2d_sprite_load_all("images/legs/legs_jump.png", 38.6, 25, 12);
		self->jumpcool = 33;
		self->frameCount = 11;
		//return;

	}
	else if (buttons[SDL_SCANCODE_D])
	{
		self->position.x += 1;
		self->sprite = gf2d_sprite_load_all("images/walk.png", 39.3, 53, 11);
		//self->sprite2 = gf2d_sprite_load_all("images/legs/test_walk.png", 38.6, 42, 12);
		self->forward.x = 1;
		if ((buttons[SDL_SCANCODE_RIGHT]))
		{
			self->sprite = gf2d_sprite_load_all("images/handgun/gun_walk.png", 57.4, 45, 10);
			if (self->projectcool <= 0)
			{
				Entity* handgun = handgun_shoot(vector2d(self->position.x + 50, self->position.y + 10), self->forward, 0);
				level_add_entity(handgun);
				self->projectcool = 15;
				self->frameCount = 10;
			}
		}
		
	}
	else if (buttons[SDL_SCANCODE_A])
	{
		self->position.x -= 1;
		//self->velocity.x = -1;
		self->sprite = gf2d_sprite_load_all("images/walk_back.png", 39.1, 53, 11);
		//gf2d_sprite_free(self->sprite2);
		self->forward.x = 0;
		if ((buttons[SDL_SCANCODE_LEFT]))
		{
			self->sprite = gf2d_sprite_load_all("images/handgun/gun_walk_flip.png", 57.4, 45, 10); //CHANGE SPRITE INSTEAD OF USING FLIP
			if (self->projectcool <= 0)
			{
				Entity* handgun = handgun_shoot(vector2d(self->position.x - 10, self->position.y + 10), self->forward,0);
				level_add_entity(handgun);
				self->projectcool = 15;
				self->frameCount = 10;
			}
		}
		//SDL_SCANCODE_UP ------------- DUE TO DRAW FUNCTION ITS TOUGH TO SHOOT UP BCS DRAW STARTS FROM TOP LEFT SO SPRITE WILL CLIP FLOOR
	}
	else {
		if (self->forward.x)
		{
			self->sprite = gf2d_sprite_load_all("images/idle.png", 37.5, 49, 4);
			self->frameCount = 4;
			//return;
			if ((buttons[SDL_SCANCODE_RIGHT]))
			{
				self->sprite = gf2d_sprite_load_all("images/handgun/idleshoot.png", 60, 45, 10);	//FIX THIS SPRITE ITS ALWAYS ABOVE GROUND
				if (self->projectcool <= 0)
				{
					Entity* handgun = handgun_shoot(vector2d(self->position.x + 50, self->position.y + 10), self->forward,0);
					level_add_entity(handgun);
					self->projectcool = 15;
					self->frameCount = 10;
				}
			}
			if (self->shotgun == 1)
			{
				self->sprite = gf2d_sprite_load_all("images/shotgun/shotgun_idle.png", 43.5, 41, 4);
				self->frameCount = 4;
			}
		}
		else if (!self->forward.x)
		{
			self->sprite = gf2d_sprite_load_all("images/idleback.png", 36.5, 49, 4);
			self->frameCount = 4;
			//return;
			if ((buttons[SDL_SCANCODE_LEFT]))
			{
				self->sprite = gf2d_sprite_load_all("images/handgun/idleshoot_flip.png", 60, 45, 10); //CHANGE SPRITE INSTEAD OF USING FLIP
				if (self->projectcool <= 0)
				{
					Entity* handgun = handgun_shoot(vector2d(self->position.x - 10, self->position.y + 10), self->forward, 0);
					level_add_entity(handgun);
					self->projectcool = 15;
					self->frameCount = 10;
				}
			}
		}
	}
	if (self->jumpcool > 0 && self->forward.x == 1)
	{
		self->sprite = gf2d_sprite_load_all("images/jump.png", 35.6, 49, 5);
		self->frameCount = 5;
		self->forward.x = 1;
		//return;

		if ((buttons[SDL_SCANCODE_RIGHT]))
		{
			self->sprite = gf2d_sprite_load_all("images/handgun/gun_jump_shoot.png", 56, 48, 10);
			if (self->projectcool <= 0)
			{
				Entity* handgun = handgun_shoot(vector2d(self->position.x + 50, self->position.y + 10), self->forward,0);
				level_add_entity(handgun);
				self->projectcool = 15;
				self->frameCount = 10;
			}
		}
		if ((buttons[SDL_SCANCODE_DOWN]))
		{
			self->sprite = gf2d_sprite_load_all("images/handgun/gun_jump_down.png", 35, 65, 6);
			if (self->projectcool <= 0)
			{
				Entity* handgun = handgun_shoot(vector2d(self->position.x + 10, self->position.y + 45), self->forward,1);
				level_add_entity(handgun);
				self->projectcool = 15;
				self->frameCount = 10;
			}
		}
		if ((buttons[SDL_SCANCODE_LEFT]))
		{
			self->sprite = gf2d_sprite_load_all("images/handgun/gun_jump_shoot_flip.png", 56, 48, 10); //CHANGE SPRITE TO CORRECT FLIP SAME AS ABOVE
			if (self->projectcool <= 0)
			{
				self->forward.x = 0;
				Entity* handgun = handgun_shoot(vector2d(self->position.x - 10, self->position.y + 10), self->forward, 0);
				level_add_entity(handgun);
				self->projectcool = 15;
				self->frameCount = 10;
			}
		}
	}
	if (self->jumpcool > 0 && buttons[SDL_SCANCODE_A])
	{
		self->sprite = gf2d_sprite_load_all("images/jump_back.png", 35.6, 49, 5);
		self->frameCount = 5;
		self->forward.x = 0;
		//return;	

	}
	if (self->jumpcool > 0 && self->forward.x == 0)
	{
		self->sprite = gf2d_sprite_load_all("images/jump_back.png", 35.6, 49, 5);
		if ((buttons[SDL_SCANCODE_RIGHT]))
		{
			self->sprite = gf2d_sprite_load_all("images/handgun/gun_jump_shoot.png", 56, 48, 10);
			if (self->projectcool <= 0)
			{
				Entity* handgun = handgun_shoot(vector2d(self->position.x + 50, self->position.y + 10), self->forward, 0);
				level_add_entity(handgun);
				self->projectcool = 15;
				self->frameCount = 10;
			}
		}
		if ((buttons[SDL_SCANCODE_DOWN]))
		{
			self->sprite = gf2d_sprite_load_all("images/handgun/gun_jump_down.png", 35, 65, 6);
			if (self->projectcool <= 0)
			{
				Entity* handgun = handgun_shoot(vector2d(self->position.x + 30, self->position.y + 45), self->forward, 1);
				level_add_entity(handgun);
				self->projectcool = 15;
				self->frameCount = 10;
			}
		}
		if ((buttons[SDL_SCANCODE_LEFT]))
		{
			self->sprite = gf2d_sprite_load_all("images/handgun/gun_jump_shoot_flip.png", 56, 48, 10); //CHANGE SPRITE TO CORRECT FLIP SAME AS ABOVE
			if (self->projectcool <= 0)
			{
				self->forward.x = 0;
				Entity* handgun = handgun_shoot(vector2d(self->position.x - 10, self->position.y + 10), self->forward, 0);
				level_add_entity(handgun);
				self->projectcool = 15;
				self->frameCount = 10;
			}
		}
		
	}
	//KNIFE
	if (buttons[SDL_SCANCODE_J])		//ADD A FLIP KNIFE SPRITE SO CAN KNIFE LEFT
	{
		self->sprite = gf2d_sprite_load_all("images/handgun/knife.png", 60, 63, 6);
		self->frameCount = 6;
		if (self->projectcool <= 0)
		{
			Shape s;
			int i, count;
			Entity* other;
			Collision* c;
			List* collisionList = NULL;
			s = gf2d_shape_rect(self->position.x + (self->forward.x * -48) + 16, self->position.y, 32, 40);
			collisionList = entity_get_clipped_entities(self, s, MONSTER_LAYER, 0);
			count = gfc_list_get_count(collisionList);
		//	slog("hit %i targets", count);
			for (i = 0; i < count; i++)
			{
				c = (Collision*)gfc_list_get_nth(collisionList, i);
				if (!c)continue;
				if (!c->body)continue;
				if (!c->body->data)continue;
				other = c->body->data;
				if (other->damage)other->damage(other, 5, self);//TODO: 5 is DAMAGE
			}
			gf2d_collision_list_free(collisionList);
			self->projectcool = 15;
		}
	}
}


int  player_damage(Entity* self, int amount, Entity* source)
{
	//slog("CRUNCH");
	self->health -= amount;
}
