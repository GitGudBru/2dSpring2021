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
	ent->health = 5;
	ent->maxHealth = 8;
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

	_player = ent;
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

	if (buttons[SDL_SCANCODE_SPACE] && (self->jumpcool <= 0))  //JUMP
	{
		self->velocity.y -= 14;
		self->jumpcool = 33;
		self->frameCount = 11;
		//return;

	}
	else if (buttons[SDL_SCANCODE_D])//NORMAL WALK RIGHT
	{
		self->position.x += 1;
		self->sprite = gf2d_sprite_load_all("images/walk.png", 39.3, 53, 11);
		self->forward.x = 1;
		if ((buttons[SDL_SCANCODE_RIGHT]))
		{
			self->sprite = gf2d_sprite_load_all("images/handgun/gun_walk.png", 57.4, 45, 10);
			if (self->machinegun == 0 && self->shotgun == 0 && self->projectcool <= 0) //HANDGUN SHOOT WALK RIGHT
			{
				Entity* handgun = handgun_shoot(vector2d(self->position.x + 50, self->position.y + 10), self->forward, 0);
				level_add_entity(handgun);
				self->projectcool = 15;
				self->frameCount = 10;
			}
		}
		if (self->shotgun == 1) //SHOTGUN WALK RIGHT
		{
			self->sprite = gf2d_sprite_load_all("images/shotgun/shotgun_walk.png", 48.1, 46, 12);
			self->frameCount = 12;
			self->forward.x = 1;
			if ((buttons[SDL_SCANCODE_RIGHT]))	//SHOTGUN SHOOT WALK RIGHT	//MAKE AN ANIMATION COOLDOWN SO IT WONT PLAY CONTINUOUSLY OR MAKE FRAMEANIMSTART
			{
				self->sprite = gf2d_sprite_load_all("images/shotgun/shotgun_walk_shoot.png", 67.2, 46, 4);
				self->frameCount = 4;

				if (self->projectcool <= 0)
				{
					Entity* shotgun = shotgun_shoot(vector2d(self->position.x + 50, self->position.y - 10), self->forward, 0);
					level_add_entity(shotgun);
					self->projectcool = 15;
					//self->frameCount = 10;
				}
			}
		}
		if (self->machinegun == 1) //SPECIAL WALK RIGHT
		{
			self->sprite = gf2d_sprite_load_all("images/heavy/fire.png", 61, 46, 12);
			self->frameCount = 12;
			self->forward.x = 1;
			if ((buttons[SDL_SCANCODE_RIGHT]))
			{
				self->sprite = gf2d_sprite_load_all("images/heavy/fire_shoot.png", 63.5, 60, 4);
				if (self->shotgun == 0 && self->projectcool <= 0) //SPECIAL SHOOT WALK RIGHT
				{
					Entity* handgun = handgun_shoot(vector2d(self->position.x + 50, self->position.y + 10), self->forward, 0);
					level_add_entity(handgun);
					self->projectcool = 3;
					self->frameCount = 4;
				}
			}

		}
		
	}
	else if (buttons[SDL_SCANCODE_A]) //NORMAL WALK LEFT
	{
		self->position.x -= 1;
		//self->velocity.x = -1;
		self->sprite = gf2d_sprite_load_all("images/walk_back.png", 39.1, 53, 11);
		//gf2d_sprite_free(self->sprite2);
		self->forward.x = 0;
		if ((buttons[SDL_SCANCODE_LEFT])) //HANDGUN SHOOT WALK LEFT
		{
			self->sprite = gf2d_sprite_load_all("images/handgun/gun_walk_flip.png", 57.4, 45, 10); //CHANGE SPRITE INSTEAD OF USING FLIP
			if (self->machinegun == 0 && self->shotgun == 0 && self->projectcool <= 0)
			{
				Entity* handgun = handgun_shoot(vector2d(self->position.x - 10, self->position.y + 10), self->forward,0);
				level_add_entity(handgun);
				self->projectcool = 15;
				self->frameCount = 10;
			}
		}
		if (self->shotgun == 1)//SHOTGUN WALK LEFT
		{
			self->sprite = gf2d_sprite_load_all("images/shotgun/shotgun_walk_flip.png", 48.1, 46, 12);
			self->frameCount = 12;
			self->forward.x = 0;
			if ((buttons[SDL_SCANCODE_LEFT]))	//SHOTGUN SHOOT WALK LEFT	//MAKE AN ANIMATION COOLDOWN SO IT WONT PLAY CONTINUOUSLY OR MAKE FRAMEANIMSTART
			{
				self->sprite = gf2d_sprite_load_all("images/shotgun/shotgun_walk_shoot_flip.png", 67.2, 46, 4);
				self->frameCount = 4;

				if (self->projectcool <= 0)
				{

					Entity* shotgun = shotgun_shoot(vector2d(self->position.x - 90, self->position.y - 10), self->forward, 0);
					level_add_entity(shotgun);
					self->projectcool = 15;
				}
			}
		}
		if (self->machinegun == 1) //SPECIAL WALK LEFT
		{
			self->sprite = gf2d_sprite_load_all("images/heavy/fire.png", 61, 46, 12);
			self->frameCount = 12;
			self->forward.x = 0;
			if ((buttons[SDL_SCANCODE_LEFT])) //SPECIAL SHOOT WALK LEFT
			{
				self->frameCount = 4;
				self->sprite = gf2d_sprite_load_all("images/heavy/fire_shoot.png", 63.5, 60, 4);
				if (self->shotgun == 0 && self->projectcool <= 0)
				{
					Entity* handgun = handgun_shoot(vector2d(self->position.x - 10, self->position.y + 10), self->forward, 0);
					level_add_entity(handgun);
					self->projectcool = 3;
				}
			}
		}
		//SDL_SCANCODE_UP ------------- DUE TO DRAW FUNCTION ITS TOUGH TO SHOOT UP BCS DRAW STARTS FROM TOP LEFT SO SPRITE WILL CLIP FLOOR
	}
	else {
		if (self->forward.x) //IDLE
		{
			self->sprite = gf2d_sprite_load_all("images/idle.png", 37.5, 49, 4);
			self->frameCount = 4;
			//return;
			if ((buttons[SDL_SCANCODE_RIGHT])) //HANDGUN IDLE SHOOT RIGHT
			{
				self->sprite = gf2d_sprite_load_all("images/handgun/idleshoot.png", 60, 45, 10);	//FIX THIS SPRITE ITS ALWAYS ABOVE GROUND
				if (self->machinegun == 0 && self->shotgun == 0 && self->projectcool <= 0)
				{
					Entity* handgun = handgun_shoot(vector2d(self->position.x + 50, self->position.y + 10), self->forward,0);
					level_add_entity(handgun);
					self->projectcool = 15;
					self->frameCount = 10;
				}
			}
			if (self->shotgun == 1)//SHOTGUN IDLE
			{
				self->sprite = gf2d_sprite_load_all("images/shotgun/shotgun_idle.png", 43.5, 41, 4);
				self->frameCount = 4;
				if ((buttons[SDL_SCANCODE_RIGHT]))	//SHOTGUN IDLE SHOOT RIGHT	//MAKE AN ANIMATION COOLDOWN SO IT WONT PLAY CONTINUOUSLY OR MAKE FRAMEANIMSTART
				{
					self->sprite = gf2d_sprite_load_all("images/shotgun/shotgun_idle_shoot.png", 68.5, 41, 4);
					self->frameCount = 4;

					if (self->projectcool <= 0)
					{
						Entity* shotgun = shotgun_shoot(vector2d(self->position.x + 50, self->position.y - 10), self->forward, 0);
						level_add_entity(shotgun);
						self->projectcool = 15;
						//self->frameCount = 10;
					}
				}
			}
			if (self->machinegun == 1) //SPECIAL IDLE
			{
				self->sprite = gf2d_sprite_load_all("images/heavy/fire.png", 61, 46, 12);
				self->frameCount = 12;
				if ((buttons[SDL_SCANCODE_RIGHT])) //SPECIAL IDLE SHOOT RIGHT
				{
					self->frameCount = 4;
					self->sprite = gf2d_sprite_load_all("images/heavy/fire_shoot.png", 63.5, 60, 4);
					if (self->shotgun == 0 && self->projectcool <= 0)
					{
						Entity* handgun = handgun_shoot(vector2d(self->position.x + 50, self->position.y + 10), self->forward, 0);
						level_add_entity(handgun);
						self->projectcool = 3;
					}
				}
			}
		}
		else if (!self->forward.x) //IDLE LEFT
		{
			self->sprite = gf2d_sprite_load_all("images/idleback.png", 36.5, 49, 4);
			self->frameCount = 4;
			//return;
			if ((buttons[SDL_SCANCODE_LEFT])) //HANDGUN IDLE SHOOT LEFT
			{
				self->sprite = gf2d_sprite_load_all("images/handgun/idleshoot_flip.png", 60, 45, 10); //CHANGE SPRITE INSTEAD OF USING FLIP
				if (self->machinegun == 0 && self->shotgun == 0 && self->projectcool <= 0)
				{
					Entity* handgun = handgun_shoot(vector2d(self->position.x - 10, self->position.y + 10), self->forward, 0);
					level_add_entity(handgun);
					self->projectcool = 15;
					self->frameCount = 10;
				}
			}
			if (self->shotgun == 1) //SHOTGUN IDLE LEFT
			{
				self->sprite = gf2d_sprite_load_all("images/shotgun/shotgun_idle_flip.png", 43.5, 41, 4);
				self->frameCount = 4;
				if ((buttons[SDL_SCANCODE_LEFT]))	//SHOTGUN IDLE SHOOT LEFT	//MAKE AN ANIMATION COOLDOWN SO IT WONT PLAY CONTINUOUSLY OR MAKE FRAMEANIMSTART
				{
					self->sprite = gf2d_sprite_load_all("images/shotgun/shotgun_idle_shoot_flip.png", 67.2, 46, 4);
					self->frameCount = 4;

					if (self->projectcool <= 0)
					{
						Entity* shotgun = shotgun_shoot(vector2d(self->position.x - 90, self->position.y - 10), self->forward, 0);
						level_add_entity(shotgun);
						self->projectcool = 15;
					}
				}
			}
			if (self->machinegun == 1) //SPECIAL IDLE LEFT
			{
				self->sprite = gf2d_sprite_load_all("images/heavy/fire.png", 61, 46, 12);
				self->frameCount = 12;
				if ((buttons[SDL_SCANCODE_LEFT])) //SPECIAL IDLE SHOOT LEFT
				{
					self->frameCount = 4;
					self->sprite = gf2d_sprite_load_all("images/heavy/fire_shoot.png", 63.5, 60, 4);
					if (self->shotgun == 0 && self->projectcool <= 0)
					{
						Entity* handgun = handgun_shoot(vector2d(self->position.x - 10, self->position.y + 10), self->forward, 0);
						level_add_entity(handgun);
						self->projectcool = 3;
					}
				}
			}
		}
	}
	if (self->jumpcool > 0 && self->forward.x == 1) //JUMPING LEFT BUT TURN RIGHT
	{
		self->sprite = gf2d_sprite_load_all("images/jump.png", 35.6, 49, 5);
		self->frameCount = 5;
		self->forward.x = 1;
		//return;

		if ((buttons[SDL_SCANCODE_RIGHT])) //HANDGUN JUMP SHOOT RIGHT
		{
			self->sprite = gf2d_sprite_load_all("images/handgun/gun_jump_shoot.png", 56, 48, 10);
			if (self->projectcool <= 0)
			{
				self->forward.x = 1;
				Entity* handgun = handgun_shoot(vector2d(self->position.x + 50, self->position.y + 10), self->forward,0);
				level_add_entity(handgun);
				self->projectcool = 15;
				self->frameCount = 10;
			}
		}
		if ((buttons[SDL_SCANCODE_DOWN]))//HANDGUN JUMP SHOOT DOWN
		{
			self->sprite = gf2d_sprite_load_all("images/handgun/gun_jump_down.png", 35, 65, 6);
			if (self->machinegun == 0 && self->shotgun == 0 && self->projectcool <= 0)
			{
				Entity* handgun = handgun_shoot(vector2d(self->position.x + 10, self->position.y + 45), self->forward,1);
				level_add_entity(handgun);
				self->projectcool = 15;
				self->frameCount = 10;
			}
		}
		if ((buttons[SDL_SCANCODE_LEFT]))//HANDGUN JUMP SHOOT LEFT
		{
			self->sprite = gf2d_sprite_load_all("images/handgun/gun_jump_shoot_flip.png", 56, 48, 10); //CHANGE SPRITE TO CORRECT FLIP SAME AS ABOVE
			if (self->machinegun == 0 && self->shotgun == 0 && self->projectcool <= 0)
			{
				self->forward.x = 0;
				Entity* handgun = handgun_shoot(vector2d(self->position.x - 10, self->position.y + 10), self->forward, 0);
				level_add_entity(handgun);
				self->projectcool = 15;
				self->frameCount = 10;
			}
		}
		if (self->shotgun == 1) //SHOTGUN JUMPING LEFT BUT TURN RIGHT
		{
			self->sprite = gf2d_sprite_load_all("images/shotgun/shotgun_jump.png", 48.1, 46, 12);
			self->frameCount = 12;
			if ((buttons[SDL_SCANCODE_RIGHT]))//SHOTGUN
			{
				self->sprite = gf2d_sprite_load_all("images/shotgun/shotgun_jump_shoot.png", 67.2, 46, 4);
				self->frameCount = 4;
				if (self->projectcool <= 0)
				{
					self->forward.x = 1;
					Entity* shotgun = shotgun_shoot(vector2d(self->position.x + 50, self->position.y - 10), self->forward, 0);
					level_add_entity(shotgun);
					self->projectcool = 15;
				}
			}
			if ((buttons[SDL_SCANCODE_DOWN]))//SHOTGUN
			{
				self->sprite = gf2d_sprite_load_all("images/shotgun/shotgun_jump_shoot_down.png", 38.5, 68, 4);
				self->frameCount = 4;
				if (self->projectcool <= 0)
				{
					Entity* shotgun = shotgun_shoot(vector2d(self->position.x - 10, self->position.y + 45), self->forward, 1);
					level_add_entity(shotgun);
					self->projectcool = 15;
				}
			}
			if ((buttons[SDL_SCANCODE_LEFT]))//SHOTGUN
			{
				self->sprite = gf2d_sprite_load_all("images/shotgun/shotgun_jump_shoot_flip.png", 67.2, 46, 4); //CHANGE SPRITE TO CORRECT FLIP SAME AS ABOVE
				self->frameCount = 4;

				if (self->projectcool <= 0)
				{
					self->forward.x = 0;
					Entity* shotgun = shotgun_shoot(vector2d(self->position.x - 90, self->position.y - 10), self->forward, 0);
					level_add_entity(shotgun);
					self->projectcool = 15;
				}
			}
		}
		if (self->machinegun == 1) //SPECIAL JUMPING LEFT BUT TURN RIGHT
		{
			self->sprite = gf2d_sprite_load_all("images/heavy/fire.png", 61, 46, 12);
			self->frameCount = 12;
			if ((buttons[SDL_SCANCODE_RIGHT])) //SPECIAL JUMP SHOOT RIGHT
			{
				self->frameCount = 4;
				self->sprite = gf2d_sprite_load_all("images/heavy/fire_shoot.png", 63.5, 60, 4);
				if (self->projectcool <= 0)
				{
					self->forward.x = 1;
					Entity* handgun = handgun_shoot(vector2d(self->position.x + 50, self->position.y + 10), self->forward, 0);
					level_add_entity(handgun);
					self->projectcool = 3;
				}
			}
			if ((buttons[SDL_SCANCODE_DOWN]))//SPECIAL JUMP SHOOT DOWN
			{
				self->frameCount = 4;
				self->sprite = gf2d_sprite_load_all("images/heavy/fire_shoot.png", 63.5, 60, 4);
				if (self->shotgun == 0 && self->projectcool <= 0)
				{
					Entity* handgun = handgun_shoot(vector2d(self->position.x + 10, self->position.y + 45), self->forward, 1);
					level_add_entity(handgun);
					self->projectcool = 3;				}
			}
			if ((buttons[SDL_SCANCODE_LEFT]))//SPECIAL JUMP SHOOT LEFT
			{
				self->frameCount = 4;
				self->sprite = gf2d_sprite_load_all("images/heavy/fire_shoot.png", 63.5, 60, 4);
				if (self->shotgun == 0 && self->projectcool <= 0)
				{
					self->forward.x = 0;
					Entity* handgun = handgun_shoot(vector2d(self->position.x - 10, self->position.y + 10), self->forward, 0);
					level_add_entity(handgun);
					self->projectcool = 3;
				}
			}
		}
	}
	if (self->jumpcool > 0 && buttons[SDL_SCANCODE_A]) //NORMAL JUMPING BACK ANIMATION
	{
		self->sprite = gf2d_sprite_load_all("images/jump_back.png", 35.6, 49, 5);
		self->frameCount = 5;
		self->forward.x = 0;
		//return;	

	}
	if (self->jumpcool > 0 && self->forward.x == 0) //JUMPING RIGHT BUT TURN LEFT
	{
		self->sprite = gf2d_sprite_load_all("images/jump_back.png", 35.6, 49, 5);
		if ((buttons[SDL_SCANCODE_RIGHT]))
		{
			self->sprite = gf2d_sprite_load_all("images/handgun/gun_jump_shoot.png", 56, 48, 10);
			if (self->machinegun == 0 && self->shotgun == 0 && self->projectcool <= 0)
			{
				self->forward.x = 1;
				Entity* handgun = handgun_shoot(vector2d(self->position.x + 50, self->position.y + 10), self->forward, 0);
				level_add_entity(handgun);
				self->projectcool = 15;
			}
		}
		if ((buttons[SDL_SCANCODE_DOWN]))
		{
			self->sprite = gf2d_sprite_load_all("images/handgun/gun_jump_down.png", 35, 65, 6);
			if (self->machinegun == 0 && self->shotgun == 0 && self->projectcool <= 0)
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
		if (self->shotgun == 1)//SHOTGUN JUMPING RIGHT TURN LEFT
		{
			self->sprite = gf2d_sprite_load_all("images/shotgun/shotgun_jump_flip.png", 48.1, 46, 12);
			self->frameCount = 12;
			if ((buttons[SDL_SCANCODE_RIGHT]))//SHOTGUN
			{
				self->sprite = gf2d_sprite_load_all("images/shotgun/shotgun_jump_shoot.png", 67.2, 46, 4);
				self->frameCount = 4;
				if (self->projectcool <= 0)
				{
					self->forward.x = 1;
					Entity* shotgun = shotgun_shoot(vector2d(self->position.x + 50, self->position.y - 10), self->forward, 0);
					level_add_entity(shotgun);
					self->projectcool = 15;
				}
			}
			if ((buttons[SDL_SCANCODE_DOWN]))//SHOTGUN
			{
				self->sprite = gf2d_sprite_load_all("images/shotgun/shotgun_jump_shoot_down.png", 38.5, 68, 4);
				self->frameCount = 4;
				if (self->projectcool <= 0)
				{
					Entity* shotgun = shotgun_shoot(vector2d(self->position.x - 30, self->position.y + 45), self->forward, 1);
					level_add_entity(shotgun);
					self->projectcool = 15;
				}
			}
			if ((buttons[SDL_SCANCODE_LEFT]))//SHOTGUN
			{
				self->sprite = gf2d_sprite_load_all("images/shotgun/shotgun_jump_shoot_flip.png", 67.2, 46, 4); //CHANGE SPRITE TO CORRECT FLIP SAME AS ABOVE
				self->frameCount = 4;

				if (self->projectcool <= 0)
				{
					self->forward.x = 0;
					Entity* shotgun = shotgun_shoot(vector2d(self->position.x - 90, self->position.y - 10), self->forward, 0);
					level_add_entity(shotgun);
					self->projectcool = 15;
				}
			}
		}
		if (self->machinegun == 1) //SPECIAL JUMPING RIGHT TURN LEFT
		{
			self->sprite = gf2d_sprite_load_all("images/heavy/fire.png", 61, 46, 12);
			self->frameCount = 12;
			if ((buttons[SDL_SCANCODE_RIGHT]))
			{
				self->frameCount = 4;
				self->sprite = gf2d_sprite_load_all("images/heavy/fire_shoot.png", 63.5, 60, 4);
				if (self->shotgun == 0 && self->projectcool <= 0)
				{
					self->forward.x = 1;
					Entity* handgun = handgun_shoot(vector2d(self->position.x + 50, self->position.y + 10), self->forward, 0);
					level_add_entity(handgun);
					self->projectcool = 3;
				}
			}
			if ((buttons[SDL_SCANCODE_DOWN]))
			{
				self->frameCount = 4;
				self->sprite = gf2d_sprite_load_all("images/heavy/fire_shoot.png", 63.5, 60, 4);
				if (self->shotgun == 0 && self->projectcool <= 0)
				{
					Entity* handgun = handgun_shoot(vector2d(self->position.x + 30, self->position.y + 45), self->forward, 1);
					level_add_entity(handgun);
					self->projectcool = 3;
				}
			}
			if ((buttons[SDL_SCANCODE_LEFT]))
			{
				self->frameCount = 4;
				self->sprite = gf2d_sprite_load_all("images/heavy/fire_shoot.png", 63.5, 60, 4);
				if (self->projectcool <= 0)
				{
					self->forward.x = 0;
					Entity* handgun = handgun_shoot(vector2d(self->position.x - 10, self->position.y + 10), self->forward, 0);
					level_add_entity(handgun);
					self->projectcool = 3;
				}
			}
		}


	}
	//KNIFE
	if (buttons[SDL_SCANCODE_J])		
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

	if (buttons[SDL_SCANCODE_G] && self->projectcool <= 0 && self->bomb > 0)
	{
		Entity* bomb = bomb_spawn(vector2d(self->position.x + 32, self->position.y + 16), self->flip);
		level_add_entity(bomb);
		self->projectcool = 15;
		self->bomb = self->bomb - 1;
	}
}


int  player_damage(Entity* self, int amount, Entity* source)
{
	//slog("CRUNCH");
	self->health -= amount;
}
