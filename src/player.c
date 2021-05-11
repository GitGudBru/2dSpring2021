#include "simple_json.h"
#include "simple_logger.h"
#include "gfc_audio.h"
#include "player.h"
#include "camera.h"
#include "gf2d_sprite.h"
#include "mount.h"
#include "mount2.h"
#include "mount2.h"
#include "box.h"
#include "weapon.h"
#include "level.h"

static Entity* _player = NULL;

Sprite* digit1;

int jumpforce = 0;
int speedforce = 0;

void player_update(Entity *self);
void player_think(Entity* self);
int  player_damage(Entity* self, int amount, Entity* source);

//static int forward = 1;


Entity* player_get()
{
	return _player;
}

//Entity *player_spawn(Vector2D position, const char* filename)
Entity *player_spawn(const char* filename)

{
	Entity *ent;
	ent = entity_new();
	if (!ent)
	{
		slog("failed to create entity for the player");
		return NULL;
	}
	ent->sprite = gf2d_sprite_load_all("images/idle.png", 37.5, 49, 4);

	
	SJson* json;
	json = sj_load(filename);
	if (!json)
	{
		slog("failed to load scene file %s", filename);
		entity_free(ent);
		return NULL;
	}

	sj_get_integer_value(sj_object_get_value(json, "health"), &ent->health);
	sj_get_integer_value(sj_object_get_value(json, "rock"), &ent->bomb);
	sj_get_float_value(sj_object_get_value(json, "position_x"), &ent->position.x);
	sj_get_float_value(sj_object_get_value(json, "position_y"), &ent->position.y);
	
	Vector2D position = vector2d(ent->position.x, ent->position.y);
	ent->frameRate = 0.1;
	ent->frameCount = 4;
	ent->update = player_update;
	ent->think = player_think;
	ent->damage = player_damage;
	//ent->health = 5;		//commented out for staging presentation
	ent->forward.x = 1;
	ent->shotgun = 0;
	ent->machinegun = 0;
	ent->ostrich = 0;
	ent->ufo = 0;
	ent->bot = 0;
	ent->score = 0;

	jumpforce = 14;	//NORMAL JUMP
	speedforce = 1;	//NORMAL SPEED

	ent->shape = gf2d_shape_rect(16, 5, 30, 40);	//SIZE OF COLLIDER
	gf2d_body_set(
		&ent->body,
		"player",
		1,
		PLAYER_LAYER,//WORLD_LAYER,
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

	_player = ent;
	level_add_entity(ent);
	return ent;
}

void player_update(Entity *self)
{
	Vector2D camera;
	Vector2D cameraSize;
	Vector2D pos = vector2d(100, 960);

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

	entity_world_snap(self);    
	entity_apply_gravity(self);
	if (self->ufo == 1)
	{
		return;
	}
	else
	{
		self->velocity.y += .5; //GRAVITY
	}

	if (self->perk1 == 10)	//PERK 1
	{
		Sound *playermusic = gfc_sound_load("moozik/bingo.mp3", 1, 4);
		gfc_sound_play(playermusic, 0, 0.05, -1, -1);
		slog("Power Up Perk 1!!!");
		jumpforce = 16;
		self->perk1 = 0;
	}

	if (self->perk2 == 10)	//PERK 2
	{
		Sound *playermusic = gfc_sound_load("moozik/bingo.mp3", 1, 4);
		gfc_sound_play(playermusic, 0, 0.05, -1, -1);
		slog("Power Up Perk 2!!!");
		//jumpforce = 20;
		speedforce = 2;
		self->perk2 = 0;
	}
	if (self->perk3 == 10)	//PERK 2
	{
		Sound *playermusic = gfc_sound_load("moozik/bingo.mp3", 1, 4);
		gfc_sound_play(playermusic, 0, 0.05, -1, -1);
		slog("Power Up Perk 3!!!");
		//jumpforce = 20;

		Entity* break2 = breakable_spawn(vector2d(self->position.x +  50, 485));
		Entity* break3 = breakable_spawn(vector2d(self->position.x + 100, 485));
		Entity* break4 = breakable_spawn(vector2d(self->position.x + 150, 485));
		Entity* break5 = breakable_spawn(vector2d(self->position.x + 200, 485));
		Entity* break6 = breakable_spawn(vector2d(self->position.x + 250, 485));

		self->perk3 = 0;
	}
	
}

void player_think(Entity *self)
{
	Uint8 *buttons = SDL_GetKeyboardState(NULL);

	if (buttons[SDL_SCANCODE_SPACE] && (self->jumpcool <= 0) && self->ufo == 0)  //JUMP
	{
		self->velocity.y -= jumpforce;
		self->jumpcool = 33;
		self->frameCount = 11;
		Sound *playermusic = gfc_sound_load("moozik/jump.mp3", 1, 2);
		gfc_sound_play(playermusic, 0, 0.05, -1, -1);
		//return;
		self->perk2 += 1;
		
	}
	else if (buttons[SDL_SCANCODE_SPACE] && self->ufo == 1)  //UFO UP
	{
		self->position.y -= 3;
	}

	else if (buttons[SDL_SCANCODE_S] && self->ufo == 1)  //UFO DOWN
	{
		self->position.y += 3;
	}
	else if ((buttons[SDL_SCANCODE_DOWN] && self->ufo == 1))//UFO SHOOT DOWN
	{
		if (self->shotgun == 0 && self->projectcool <= 0)
		{
			Entity* handgun = handgun_shoot(vector2d(self->position.x + 10, self->position.y + 45), self->forward, 1, MONSTER_LAYER);
			level_add_entity(handgun);
			self->projectcool = 3;
			Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
			gfc_sound_play(playermusic, 0, 0.05, -1, -1);
		}
	}
	else if (buttons[SDL_SCANCODE_D])//NORMAL WALK RIGHT
	{
		self->position.x += speedforce;
		self->frameCount = 10;
		self->sprite = gf2d_sprite_load_all("images/walk.png", 39.3, 53, 11);
		self->forward.x = 1;
		if ((buttons[SDL_SCANCODE_RIGHT]))
		{
			self->sprite = gf2d_sprite_load_all("images/handgun/gun_walk.png", 57.4, 45, 10);
			if (self->machinegun == 0 && self->shotgun == 0 && self->ostrich == 0 && self->ufo == 0 && self->bot == 0 && self->projectcool <= 0) //HANDGUN SHOOT WALK RIGHT
			{
				Entity* handgun = handgun_shoot(vector2d(self->position.x + 50, self->position.y + 10), self->forward, 0, MONSTER_LAYER);
				level_add_entity(handgun);
				self->projectcool = 15;
				self->frameCount = 10;
				Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
				gfc_sound_play(playermusic, 0, 0.05, -1, -1);
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
					Sound *playermusic = gfc_sound_load("moozik/shotgun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
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
					Entity* handgun = handgun_shoot(vector2d(self->position.x + 50, self->position.y + 10), self->forward, 0, MONSTER_LAYER);
					level_add_entity(handgun);
					self->projectcool = 3;
					self->frameCount = 4;
					Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
				}
			}
		}
		if (self->ufo == 1) //UFO WALK RIGHT
		{
			self->sprite = gf2d_sprite_load_all("images/ufo/ufo.png", 50, 40, 16);
			self->frameCount = 16;
			self->forward.x = 1;
			if ((buttons[SDL_SCANCODE_RIGHT]))
			{
				return;
			}
		}
		if (self->bot == 1) //BOT WALK RIGHT
		{
			self->sprite = gf2d_sprite_load_all("images/bot/bot_run_right.png", 56, 61, 16);
			self->frameCount = 16;
			self->forward.x = 1;
			if ((buttons[SDL_SCANCODE_RIGHT]))
			{
				return;
			}
		}
		if (self->ostrich == 1) //OSTRICH WALK RIGHT
		{
			self->sprite = gf2d_sprite_load_all("images/ostrich/os_run_right.png", 67, 61, 12);
			self->frameCount = 12;
			self->forward.x = 1;
			self->position.x += 1;
			if ((buttons[SDL_SCANCODE_RIGHT]))
			{
				self->sprite = gf2d_sprite_load_all("images/ostrich/os_run_right.png", 67, 61, 12);
				if (self->shotgun == 0 && self->projectcool <= 0) //OSTRICH SHOOT RIGHT WALK RIGHT
				{
					Entity* handgun = handgun_shoot(vector2d(self->position.x + 60, self->position.y + 30), self->forward, 0, MONSTER_LAYER);
					level_add_entity(handgun);
					self->projectcool = 3;
					self->frameCount = 12;
					Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
				}
			}
			if ((buttons[SDL_SCANCODE_LEFT])) //OSTRICH SHOOT LEFT WALK RIGHT
			{
				self->forward.x = 0;
				self->frameCount = 12;
				self->sprite = gf2d_sprite_load_all("images/ostrich/os_run_right_shoot_left.png", 67, 61, 12);
				if (self->shotgun == 0 && self->projectcool <= 0)
				{
					Entity* handgun = handgun_shoot(vector2d(self->position.x - 10, self->position.y + 30), self->forward, 0, MONSTER_LAYER);
					level_add_entity(handgun);
					self->projectcool = 3;
					Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
				}
			}
			if ((buttons[SDL_SCANCODE_UP]))
			{
				self->sprite = gf2d_sprite_load_all("images/ostrich/os_run_right_shoot_up.png", 67, 61, 12);
				if (self->shotgun == 0 && self->projectcool <= 0) //OSTRICH SHOOT UP WALK RIGHT
				{
					Entity* handgun = handgun_shoot(vector2d(self->position.x + 40, self->position.y + 10), self->forward, -1, MONSTER_LAYER);
					level_add_entity(handgun);
					self->projectcool = 3;
					Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
				}
			}

		}
		
	}
	else if (buttons[SDL_SCANCODE_A]) //NORMAL WALK LEFT
	{
		self->position.x -= speedforce;
		//self->velocity.x = -1;
		self->sprite = gf2d_sprite_load_all("images/walk_back.png", 39.1, 53, 11);
		//gf2d_sprite_free(self->sprite2);
		self->forward.x = 0;
		if ((buttons[SDL_SCANCODE_LEFT])) //HANDGUN SHOOT WALK LEFT
		{
			self->sprite = gf2d_sprite_load_all("images/handgun/gun_walk_flip.png", 57.4, 45, 10); //CHANGE SPRITE INSTEAD OF USING FLIP
			if (self->machinegun == 0 && self->shotgun == 0 && self->ostrich == 0 && self->ufo == 0 && self->bot == 0 && self->projectcool <= 0)
			{
				Entity* handgun = handgun_shoot(vector2d(self->position.x - 10, self->position.y + 10), self->forward, 0, MONSTER_LAYER);
				level_add_entity(handgun);
				self->projectcool = 15;
				self->frameCount = 10;
				Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
				gfc_sound_play(playermusic, 0, 0.05, -1, -1);
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
					Sound *playermusic = gfc_sound_load("moozik/shotgun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
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
					Entity* handgun = handgun_shoot(vector2d(self->position.x - 10, self->position.y + 10), self->forward, 0, MONSTER_LAYER);
					level_add_entity(handgun);
					self->projectcool = 3;
					Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
				}
			}
		}
		if (self->ufo == 1) //UFO WALK LEFT
		{
			self->sprite = gf2d_sprite_load_all("images/ufo/ufo.png", 50, 40, 16);
			self->frameCount = 16;
			self->forward.x = 0;
			if ((buttons[SDL_SCANCODE_LEFT]))
			{
				return;
			}
		}
		if (self->bot == 1) //BOT WALK LEFT
		{
			self->sprite = gf2d_sprite_load_all("images/bot/bot_run_left.png", 56, 61, 16);
			self->frameCount = 16;
			self->forward.x = 1;
			if ((buttons[SDL_SCANCODE_LEFT]))
			{
				return;
			}
		}
		if (self->ostrich == 1) //OSTRICH WALK LEFT
		{
			self->sprite = gf2d_sprite_load_all("images/ostrich/os_run_left_shoot_right.png", 67, 61, 12);
			self->frameCount = 12;
			self->forward.x = 0;
			if ((buttons[SDL_SCANCODE_LEFT])) //OSTRICH SHOOT LEFT WALK LEFT
			{
				self->sprite = gf2d_sprite_load_all("images/ostrich/os_run_left_shoot_left.png", 67, 61, 12);
				if (self->shotgun == 0 && self->projectcool <= 0)
				{
					Entity* handgun = handgun_shoot(vector2d(self->position.x - 10, self->position.y + 30), self->forward, 0, MONSTER_LAYER);
					level_add_entity(handgun);
					self->projectcool = 3;
					Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
				}
			}
			if ((buttons[SDL_SCANCODE_RIGHT])) //OSTRICH SHOOT RIGHT WALK LEFT
			{
				self->forward.x = 1;
				self->sprite = gf2d_sprite_load_all("images/ostrich/os_run_left_shoot_right.png", 67, 61, 12);
				if (self->shotgun == 0 && self->projectcool <= 0)
				{
					Entity* handgun = handgun_shoot(vector2d(self->position.x + 60, self->position.y + 30), self->forward, 0, MONSTER_LAYER);
					level_add_entity(handgun);
					self->projectcool = 3;
					Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
				}
			}
			if ((buttons[SDL_SCANCODE_UP]))
			{
				self->sprite = gf2d_sprite_load_all("images/ostrich/os_run_left_shoot_up.png", 67, 61, 12);
				if (self->shotgun == 0 && self->projectcool <= 0) //OSTRICH SHOOT UP WALK RIGHT
				{
					Entity* handgun = handgun_shoot(vector2d(self->position.x + 40, self->position.y + 10), self->forward, -1, MONSTER_LAYER);
					level_add_entity(handgun);
					self->projectcool = 3;
					Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
				}
			}
		}

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
				if (self->machinegun == 0 && self->shotgun == 0 && self->ostrich == 0 && self->ufo == 0 && self->bot == 0 && self->projectcool <= 0)
				{
					Entity* handgun = handgun_shoot(vector2d(self->position.x + 50, self->position.y + 10), self->forward, 0, MONSTER_LAYER);
					level_add_entity(handgun);
					self->projectcool = 15;
					self->frameCount = 10;
					Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
					self->perk1 += 1;
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
						Sound *playermusic = gfc_sound_load("moozik/shotgun.mp3", 1, 2);
						gfc_sound_play(playermusic, 0, 0.05, -1, -1);
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
						Entity* handgun = handgun_shoot(vector2d(self->position.x + 50, self->position.y + 10), self->forward, 0, MONSTER_LAYER);
						level_add_entity(handgun);
						self->projectcool = 3;
						Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
						gfc_sound_play(playermusic, 0, 0.05, -1, -1);
					}
				}
			}
			if (self->ufo == 1) //UFO IDLE
			{
				self->sprite = gf2d_sprite_load_all("images/ufo/ufo.png", 50, 40, 16);
				self->frameCount = 16;
				if ((buttons[SDL_SCANCODE_RIGHT]))
				{
					return;

				}
			}
			if (self->bot == 1) //BOT IDLE
			{
				self->sprite = gf2d_sprite_load_all("images/bot/bot_idle.png", 62, 58, 4);
				self->frameCount = 4;
				if ((buttons[SDL_SCANCODE_RIGHT])) //BOT IDLE SHOOT RIGHT
				{
					self->frameCount = 4;
					self->sprite = gf2d_sprite_load_all("images/bot/bot_fire_right.png", 99, 60, 4);
					if (self->shotgun == 0 && self->projectcool <= 0)
					{
						Entity* handgun = handgun_shoot(vector2d(self->position.x + 50, self->position.y + 20), self->forward, 0, MONSTER_LAYER);
						level_add_entity(handgun);
						self->projectcool = 3;
						Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
						gfc_sound_play(playermusic, 0, 0.05, -1, -1);
					}
				}
				if ((buttons[SDL_SCANCODE_UP]))
				{
					self->frameCount = 4;
					self->sprite = gf2d_sprite_load_all("images/bot/bot_shoot_up2.png", 62, 62, 4);
					if (self->shotgun == 0 && self->projectcool <= 0) //BOT SHOOT UP IDLE
					{
						Entity* handgun = handgun_shoot(vector2d(self->position.x + 40, self->position.y ), self->forward, -1, MONSTER_LAYER);
						level_add_entity(handgun);
						self->projectcool = 3;
						Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
						gfc_sound_play(playermusic, 0, 0.05, -1, -1);
					}
				}
			}
			if (self->ostrich == 1) //OSTRICH IDLE
			{
				self->sprite = gf2d_sprite_load_all("images/ostrich/os_idle.png", 60, 62, 6);
				self->frameCount = 6;
				if ((buttons[SDL_SCANCODE_LEFT])) //OSTRICH iDLE LEFT WALK LEFT
				{
					self->forward.x = 0;
					self->sprite = gf2d_sprite_load_all("images/ostrich/os_idle_shoot_left.png", 60, 61, 6);
					if (self->shotgun == 0 && self->projectcool <= 0)
					{
						Entity* handgun = handgun_shoot(vector2d(self->position.x - 10, self->position.y + 30), self->forward, 0, MONSTER_LAYER);
						level_add_entity(handgun);
						self->projectcool = 3;
						Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
						gfc_sound_play(playermusic, 0, 0.05, -1, -1);
					}
				}
				if ((buttons[SDL_SCANCODE_RIGHT])) //OSTRICH IDLE RIGHT WALK LEFT
				{
					if (self->shotgun == 0 && self->projectcool <= 0)
					{
						Entity* handgun = handgun_shoot(vector2d(self->position.x + 60, self->position.y + 30), self->forward, 0, MONSTER_LAYER);
						level_add_entity(handgun);
						self->projectcool = 3;
						Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
						gfc_sound_play(playermusic, 0, 0.05, -1, -1);
					}
				}
				if ((buttons[SDL_SCANCODE_UP]))
				{
					self->sprite = gf2d_sprite_load_all("images/ostrich/os_idle_shoot_up.png", 60, 61, 6);
					if (self->shotgun == 0 && self->projectcool <= 0) //OSTRICH IDLE SHOOT UP 
					{
						Entity* handgun = handgun_shoot(vector2d(self->position.x + 30, self->position.y + 10), self->forward, -1, MONSTER_LAYER);
						level_add_entity(handgun);
						self->projectcool = 3;
						Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
						gfc_sound_play(playermusic, 0, 0.05, -1, -1);
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
				if (self->machinegun == 0 && self->shotgun == 0 && self->ostrich == 0 && self->ufo == 0 && self->bot == 0 && self->projectcool <= 0)
				{
					Entity* handgun = handgun_shoot(vector2d(self->position.x - 10, self->position.y + 10), self->forward, 0, MONSTER_LAYER);
					level_add_entity(handgun);
					self->projectcool = 15;
					self->frameCount = 10;
					Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
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
						Sound *playermusic = gfc_sound_load("moozik/shotgun.mp3", 1, 2);
						gfc_sound_play(playermusic, 0, 0.05, -1, -1);
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
						Entity* handgun = handgun_shoot(vector2d(self->position.x - 10, self->position.y + 10), self->forward, 0, MONSTER_LAYER);
						level_add_entity(handgun);
						self->projectcool = 3;
						Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
						gfc_sound_play(playermusic, 0, 0.05, -1, -1);
					}
				}
			}
			if (self->ufo == 1) //UFO IDLE LEFT
			{
				self->sprite = gf2d_sprite_load_all("images/ufo/ufo.png", 50, 40, 16);
				self->frameCount = 16;
				if ((buttons[SDL_SCANCODE_LEFT]))
				{
					return;

				}
			}
			if (self->bot == 1) //BOT IDLE LEFT
			{
				self->sprite = gf2d_sprite_load_all("images/bot/bot_idle.png", 62, 58, 4);
				self->frameCount = 4;
				if ((buttons[SDL_SCANCODE_LEFT]))
				{
					return;
				}
			}
			if (self->ostrich == 1) //OSTRICH IDLE LEFT
			{
				self->sprite = gf2d_sprite_load_all("images/ostrich/os_idle.png", 60, 62, 6);
				self->frameCount = 6;
				if ((buttons[SDL_SCANCODE_LEFT])) //OSTRICH iDLE LEFT WALK LEFT
				{
					self->frameCount = 6;
					self->sprite = gf2d_sprite_load_all("images/ostrich/os_idle_shoot_left.png", 60, 61, 6);
					if (self->shotgun == 0 && self->projectcool <= 0)
					{
						Entity* handgun = handgun_shoot(vector2d(self->position.x - 10, self->position.y + 30), self->forward, 0, MONSTER_LAYER);
						level_add_entity(handgun);
						self->projectcool = 3;
						Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
						gfc_sound_play(playermusic, 0, 0.05, -1, -1);
					}
				}
				if ((buttons[SDL_SCANCODE_RIGHT])) //OSTRICH IDLE RIGHT WALK LEFT
				{
					self->forward.x = 1;
					if (self->shotgun == 0 && self->projectcool <= 0)
					{
						Entity* handgun = handgun_shoot(vector2d(self->position.x + 60, self->position.y + 30), self->forward, 0, MONSTER_LAYER);
						level_add_entity(handgun);
						self->projectcool = 3;
						Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
						gfc_sound_play(playermusic, 0, 0.05, -1, -1);
					}
				}
				if ((buttons[SDL_SCANCODE_UP]))
				{
					self->frameCount = 6;
					self->sprite = gf2d_sprite_load_all("images/ostrich/os_idle_shoot_up.png", 60, 61, 6);
					if (self->shotgun == 0 && self->projectcool <= 0) //OSTRICH IDLE SHOOT UP 
					{
						Entity* handgun = handgun_shoot(vector2d(self->position.x + 30, self->position.y + 10), self->forward, -1, MONSTER_LAYER);
						level_add_entity(handgun);
						self->projectcool = 3;
						Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
						gfc_sound_play(playermusic, 0, 0.05, -1, -1);
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
				Entity* handgun = handgun_shoot(vector2d(self->position.x + 50, self->position.y + 10), self->forward, 0, MONSTER_LAYER);
				level_add_entity(handgun);
				self->projectcool = 15;
				self->frameCount = 10;
				Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
				gfc_sound_play(playermusic, 0, 0.05, -1, -1);
			}
		}
		if ((buttons[SDL_SCANCODE_DOWN]))//HANDGUN JUMP SHOOT DOWN
		{
			self->sprite = gf2d_sprite_load_all("images/handgun/gun_jump_down.png", 35, 65, 6);
			if (self->machinegun == 0 && self->shotgun == 0 && self->ostrich == 0 && self->ufo == 0 && self->bot == 0 && self->projectcool <= 0)
			{
				Entity* handgun = handgun_shoot(vector2d(self->position.x + 10, self->position.y + 45), self->forward, 1, MONSTER_LAYER);
				level_add_entity(handgun);
				self->projectcool = 15;
				self->frameCount = 10;
				Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
				gfc_sound_play(playermusic, 0, 0.05, -1, -1);
			}
		}
		if ((buttons[SDL_SCANCODE_LEFT]))//HANDGUN JUMP SHOOT LEFT
		{
			self->sprite = gf2d_sprite_load_all("images/handgun/gun_jump_shoot_flip.png", 56, 48, 10); //CHANGE SPRITE TO CORRECT FLIP SAME AS ABOVE
			if (self->machinegun == 0 && self->shotgun == 0 && self->ostrich == 0 && self->ufo == 0 && self->bot == 0 && self->projectcool <= 0)
			{
				self->forward.x = 0;
				Entity* handgun = handgun_shoot(vector2d(self->position.x - 10, self->position.y + 10), self->forward, 0, MONSTER_LAYER);
				level_add_entity(handgun);
				self->projectcool = 15;
				self->frameCount = 10;
				Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
				gfc_sound_play(playermusic, 0, 0.05, -1, -1);
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
					Sound *playermusic = gfc_sound_load("moozik/shotgun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
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
					Sound *playermusic = gfc_sound_load("moozik/shotgun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
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
					Sound *playermusic = gfc_sound_load("moozik/shotgun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
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
					Entity* handgun = handgun_shoot(vector2d(self->position.x + 50, self->position.y + 10), self->forward, 0, MONSTER_LAYER);
					level_add_entity(handgun);
					self->projectcool = 3;
					Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
				}
			}
			if ((buttons[SDL_SCANCODE_DOWN]))//SPECIAL JUMP SHOOT DOWN
			{
				self->frameCount = 4;
				self->sprite = gf2d_sprite_load_all("images/heavy/fire_shoot.png", 63.5, 60, 4);
				if (self->shotgun == 0 && self->projectcool <= 0)
				{
					Entity* handgun = handgun_shoot(vector2d(self->position.x + 10, self->position.y + 45), self->forward, 1, MONSTER_LAYER);
					level_add_entity(handgun);
					self->projectcool = 3;		
					Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
				}
			}
			if ((buttons[SDL_SCANCODE_LEFT]))//SPECIAL JUMP SHOOT LEFT
			{
				self->frameCount = 4;
				self->sprite = gf2d_sprite_load_all("images/heavy/fire_shoot.png", 63.5, 60, 4);
				if (self->shotgun == 0 && self->projectcool <= 0)
				{
					self->forward.x = 0;
					Entity* handgun = handgun_shoot(vector2d(self->position.x - 10, self->position.y + 10), self->forward, 0, MONSTER_LAYER);
					level_add_entity(handgun);
					self->projectcool = 3;
					Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
				}
			}
		}
		if (self->ufo == 1) //UFO JUMPING LEFT BUT TURN RIGHT
		{
			self->sprite = gf2d_sprite_load_all("images/ufo/ufo.png", 50, 40, 16);
			self->frameCount = 16;
			if ((buttons[SDL_SCANCODE_RIGHT])) //UFO JUMP SHOOT RIGHT
			{
				self->forward.x = 1;
				return;
			}
			if ((buttons[SDL_SCANCODE_DOWN]))//UFO JUMP SHOOT DOWN
			{
				if (self->shotgun == 0 && self->projectcool <= 0)
				{
					Entity* handgun = handgun_shoot(vector2d(self->position.x + 10, self->position.y + 45), self->forward, 1, MONSTER_LAYER);
					level_add_entity(handgun);
					self->projectcool = 3;
					Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
				}
			}
			if ((buttons[SDL_SCANCODE_LEFT]))//UFO JUMP SHOOT LEFT
			{
				self->forward.x = 0;
				return;
			}
		}
		if (self->bot == 1) //BOT JUMPING LEFT BUT TURN RIGHT
		{
			self->sprite = gf2d_sprite_load_all("images/bot/bot_jump2.png", 63, 67, 5);
			self->frameCount = 5;
			if ((buttons[SDL_SCANCODE_RIGHT])) //BOT JUMP SHOOT RIGHT
			{
				self->forward.x = 1;
				return;
			}
			if ((buttons[SDL_SCANCODE_DOWN]))//BOT JUMP SHOOT DOWN
			{
				self->sprite = gf2d_sprite_load_all("images/bot/bot_jump_shoot_down.png", 61, 88, 4);
				self->frameCount = 4;
				if (self->shotgun == 0 && self->projectcool <= 0)
				{
					Entity* handgun = handgun_shoot(vector2d(self->position.x + 40, self->position.y + 45), self->forward, 1, MONSTER_LAYER);
					level_add_entity(handgun);
					self->projectcool = 3;
					Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
				}
			}
			if ((buttons[SDL_SCANCODE_LEFT]))//BOT JUMP SHOOT LEFT
			{
				self->forward.x = 1;
				return;
			}
		}
		if (self->ostrich == 1) //OSTRICH JUMPING LEFT BUT TURN RIGHT
		{
			self->sprite = gf2d_sprite_load_all("images/ostrich/os_jump.png", 62, 74, 11);
			self->frameCount = 11;
			if ((buttons[SDL_SCANCODE_RIGHT])) //OSTRICH JUMP SHOOT RIGHT
			{
				self->sprite = gf2d_sprite_load_all("images/ostrich/os_jump.png", 62, 74, 11);
				self->frameCount = 11;
				if (self->projectcool <= 0)
				{
					self->forward.x = 1;
					Entity* handgun = handgun_shoot(vector2d(self->position.x + 50, self->position.y + 10), self->forward, 0, MONSTER_LAYER);
					level_add_entity(handgun);
					self->projectcool = 3;
					Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
				}
			}
			if ((buttons[SDL_SCANCODE_DOWN]))//OSTRICH JUMP SHOOT DOWN
			{
				self->frameCount = 11;
				self->sprite = gf2d_sprite_load_all("images/ostrich/os_jump_shoot_down.png", 62, 74, 11);
				if (self->shotgun == 0 && self->projectcool <= 0)
				{
					Entity* handgun = handgun_shoot(vector2d(self->position.x + 30, self->position.y + 45), self->forward, 1, MONSTER_LAYER);
					level_add_entity(handgun);
					self->projectcool = 3;
					Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
				}
			}
			if ((buttons[SDL_SCANCODE_LEFT]))//OSTRICH JUMP SHOOT LEFT
			{
				self->frameCount = 11;
				self->sprite = gf2d_sprite_load_all("images/ostrich/os_jump_shoot_left.png", 62, 74, 11);
				if (self->shotgun == 0 && self->projectcool <= 0)
				{
					self->forward.x = 0;
					Entity* handgun = handgun_shoot(vector2d(self->position.x - 10, self->position.y + 10), self->forward, 0, MONSTER_LAYER);
					level_add_entity(handgun);
					self->projectcool = 3;
					Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
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
			if (self->machinegun == 0 && self->shotgun == 0 && self->ostrich == 0 && self->ufo == 0 && self->bot == 0 && self->projectcool <= 0)
			{
				self->forward.x = 1;
				Entity* handgun = handgun_shoot(vector2d(self->position.x + 50, self->position.y + 10), self->forward, 0, MONSTER_LAYER);
				level_add_entity(handgun);
				self->projectcool = 15;
				Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
				gfc_sound_play(playermusic, 0, 0.05, -1, -1);
			}
		}
		if ((buttons[SDL_SCANCODE_DOWN]))
		{
			self->sprite = gf2d_sprite_load_all("images/handgun/gun_jump_down.png", 35, 65, 6);
			if (self->machinegun == 0 && self->shotgun == 0 && self->ostrich == 0 && self->ufo == 0 && self->bot == 0 && self->projectcool <= 0)
			{
				Entity* handgun = handgun_shoot(vector2d(self->position.x + 30, self->position.y + 45), self->forward, 1, MONSTER_LAYER);
				level_add_entity(handgun);
				self->projectcool = 15;
				self->frameCount = 10;
				Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
				gfc_sound_play(playermusic, 0, 0.05, -1, -1);
			}
		}
		if ((buttons[SDL_SCANCODE_LEFT]))
		{
			self->sprite = gf2d_sprite_load_all("images/handgun/gun_jump_shoot_flip.png", 56, 48, 10); //CHANGE SPRITE TO CORRECT FLIP SAME AS ABOVE
			if (self->projectcool <= 0)
			{
				self->forward.x = 0;
				Entity* handgun = handgun_shoot(vector2d(self->position.x - 10, self->position.y + 10), self->forward, 0, MONSTER_LAYER);
				level_add_entity(handgun);
				self->projectcool = 15;
				self->frameCount = 10;
				Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
				gfc_sound_play(playermusic, 0, 0.05, -1, -1);
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
					Sound *playermusic = gfc_sound_load("moozik/shotgun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
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
					Sound *playermusic = gfc_sound_load("moozik/shotgun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
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
					Sound *playermusic = gfc_sound_load("moozik/shotgun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
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
					Entity* handgun = handgun_shoot(vector2d(self->position.x + 50, self->position.y + 10), self->forward, 0, MONSTER_LAYER);
					level_add_entity(handgun);
					self->projectcool = 3;
					Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
				}
			}
			if ((buttons[SDL_SCANCODE_DOWN]))
			{
				self->frameCount = 4;
				self->sprite = gf2d_sprite_load_all("images/heavy/fire_shoot.png", 63.5, 60, 4);
				if (self->shotgun == 0 && self->projectcool <= 0)
				{
					Entity* handgun = handgun_shoot(vector2d(self->position.x + 30, self->position.y + 45), self->forward, 1, MONSTER_LAYER);
					level_add_entity(handgun);
					self->projectcool = 3;
					Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
				}
			}
			if ((buttons[SDL_SCANCODE_LEFT]))
			{
				self->frameCount = 4;
				self->sprite = gf2d_sprite_load_all("images/heavy/fire_shoot.png", 63.5, 60, 4);
				if (self->projectcool <= 0)
				{
					self->forward.x = 0;
					Entity* handgun = handgun_shoot(vector2d(self->position.x - 10, self->position.y + 10), self->forward, 0, MONSTER_LAYER);
					level_add_entity(handgun);
					self->projectcool = 3;
					Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
				}
			}
		}
		if (self->ufo == 1) //UFO JUMPING RIGHT TURN LEFT
		{
			self->sprite = gf2d_sprite_load_all("images/ufo/ufo.png", 50, 40, 16);
			self->frameCount = 16;
			if ((buttons[SDL_SCANCODE_RIGHT]))//UFO
			{
				self->forward.x = 1;
				return;
			}
			if ((buttons[SDL_SCANCODE_DOWN]))//UFO
			{
				if (self->shotgun == 0 && self->projectcool <= 0)
				{
					Entity* handgun = handgun_shoot(vector2d(self->position.x + 30, self->position.y + 45), self->forward, 1, MONSTER_LAYER);
					level_add_entity(handgun);
					self->projectcool = 3;
					Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
				}
			}
			if ((buttons[SDL_SCANCODE_LEFT]))//UFO
			{
				return;
			}
		}
		if (self->bot == 1) //BOT JUMPING LEFT BUT TURN RIGHT
		{
			self->sprite = gf2d_sprite_load_all("images/bot/bot_jump2.png", 63, 67, 5);
			self->frameCount = 5;
			if ((buttons[SDL_SCANCODE_RIGHT])) //BOT JUMP SHOOT RIGHT
			{
				self->forward.x = 1;
				return;
			}
			if ((buttons[SDL_SCANCODE_DOWN]))//BOT JUMP SHOOT DOWN
			{
				self->sprite = gf2d_sprite_load_all("images/bot/bot_jump_shoot_down.png", 61, 88, 4);
				self->frameCount = 4;
				if (self->shotgun == 0 && self->projectcool <= 0)
				{
					Entity* handgun = handgun_shoot(vector2d(self->position.x + 10, self->position.y + 45), self->forward, 1, MONSTER_LAYER);
					level_add_entity(handgun);
					self->projectcool = 3;
					Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
				}
			}
			if ((buttons[SDL_SCANCODE_LEFT]))//BOT JUMP SHOOT LEFT
			{
				self->forward.x = 1;
				return;
			}
		}
		if (self->ostrich == 1) //OSTRICH JUMPING RIGHT TURN LEFT
		{
			self->sprite = gf2d_sprite_load_all("images/ostrich/os_jump.png", 62, 74, 11);
			self->frameCount = 11;
			if ((buttons[SDL_SCANCODE_RIGHT]))//OSTRICH
			{
				self->sprite = gf2d_sprite_load_all("images/ostrich/os_jump.png", 62, 74, 11);
				self->frameCount = 11;
				if (self->shotgun == 0 && self->projectcool <= 0)
				{
					self->forward.x = 1;
					Entity* handgun = handgun_shoot(vector2d(self->position.x + 50, self->position.y + 10), self->forward, 0, MONSTER_LAYER);
					level_add_entity(handgun);
					self->projectcool = 3;
					Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
				}
			}
			if ((buttons[SDL_SCANCODE_DOWN]))//OSTRICH
			{
				self->frameCount = 11;
				self->sprite = gf2d_sprite_load_all("images/ostrich/os_jump_shoot_down.png", 62, 74, 11);
				if (self->shotgun == 0 && self->projectcool <= 0)
				{
					Entity* handgun = handgun_shoot(vector2d(self->position.x + 30, self->position.y + 45), self->forward, 1, MONSTER_LAYER);
					level_add_entity(handgun);
					self->projectcool = 3;
					Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
				}
			}
			if ((buttons[SDL_SCANCODE_LEFT]))//OSTRICH
			{
				self->frameCount = 11;
				self->sprite = gf2d_sprite_load_all("images/ostrich/os_jump_shoot_left.png", 62, 74, 11);
				if (self->projectcool <= 0)
				{
					self->forward.x = 0;
					Entity* handgun = handgun_shoot(vector2d(self->position.x - 10, self->position.y + 10), self->forward, 0, MONSTER_LAYER);
					level_add_entity(handgun);
					self->projectcool = 3;
					Sound *playermusic = gfc_sound_load("moozik/gun.mp3", 1, 2);
					gfc_sound_play(playermusic, 0, 0.05, -1, -1);
				}
			}
		}
	}
	//KNIFE
	if (buttons[SDL_SCANCODE_J] && self->bot == 0)
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

	if (buttons[SDL_SCANCODE_J] && self->bot == 1)
	{
		self->sprite = gf2d_sprite_load_all("images/bot/bot_knife.png", 74, 60, 10);
		self->frameCount = 10;
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
				if (other->damage)other->damage(other, 25, self);//TODO: 5 is DAMAGE		
			}
			gf2d_collision_list_free(collisionList);
			self->projectcool = 15;
		}
	}

	if (buttons[SDL_SCANCODE_G] && self->projectcool <= 0 && self->bomb > 0) //GRENADE
	{
		Entity* bomb = bomb_spawn(vector2d(self->position.x + 32, self->position.y + 16), self->flip, MONSTER_LAYER, 0.7);
		level_add_entity(bomb);
		self->projectcool = 15;
		self->bomb = self->bomb - 1;
		self->perk3 += 1;
	}
	if (buttons[SDL_SCANCODE_1]) //STAGING
	{
		self-> shotgun = 1;
		self->machinegun = 0;
	}
	if (buttons[SDL_SCANCODE_2]) //STAGING
	{
		self->shotgun = 0;
		self->machinegun = 1;

	}
	if (buttons[SDL_SCANCODE_3]) //STAGING
	{
		self->shotgun = 0;
		self->machinegun = 0;
	}

	if (buttons[SDL_SCANCODE_M]) 
	{
		if (self->ostrich == 1)
		{
			self->ostrich = 0;
			self->shape = gf2d_shape_rect(16, 5, 30, 40);	//SIZE OF COLLIDER
			gf2d_body_set(
				&self->body,
				"player",
				1,
				PLAYER_LAYER,//WORLD_LAYER,
				0,
				1,
				self->position,
				vector2d(0, 0),
				10,
				1,
				0,
				&self->shape,
				self,
				NULL);

			mount_spawn(vector2d(self->position.x - 50, self->position.y));
		}		
		else if (self->ufo == 1)
		{
			self->ufo = 0;
			self->shape = gf2d_shape_rect(16, 5, 30, 40);	//SIZE OF COLLIDER
			gf2d_body_set(
				&self->body,
				"player",
				1,
				PLAYER_LAYER,//WORLD_LAYER,
				0,
				1,
				self->position,
				vector2d(0, 0),
				10,
				1,
				0,
				&self->shape,
				self,
				NULL);
			mount2_spawn(vector2d(self->position.x - 50, self->position.y));
		}
		else if (self->bot == 1)
		{
			self->bot = 0;
			self->shape = gf2d_shape_rect(16, 5, 30, 40);	//SIZE OF COLLIDER
			gf2d_body_set(
				&self->body,
				"player",
				1,
				PLAYER_LAYER,//WORLD_LAYER,
				0,
				1,
				self->position,
				vector2d(0, 0),
				10,
				1,
				0,
				&self->shape,
				self,
				NULL);
			mount3_spawn(vector2d(self->position.x - 50, self->position.y));
		}
	}

}

SJson* player_to_json(Entity* self)
{
	SJson* json;
	if (!self)return NULL;

	json = sj_object_new();
	if (!json)return NULL;

	sj_object_insert(json, "pickup score:", sj_new_int(self->score));
	sj_object_insert(json, "Defeated 1st Boss:", sj_new_int(self->perk4));
	sj_object_insert(json, "Defeated 2nd Boss:", sj_new_int(self->perk5));

	return json;
}

void player_save(Entity* self, char* filename)
{
	SJson* json;
	if (!self)return;

	json = player_to_json(self);
	if (!json)return;
	sj_save(json, filename);
}


void player_health_image_set(Entity* self, Vector2D position)
{
	Vector2D pos = position;
	char *img; //= gf2d_sprite_load_all("images/health5.png");

	if(self->health == 5)
	{
		img = "images/health5.png";
	}
	else if (self->health == 4)
	{
		img = "images/health4.png";
	}
	else if (self->health == 3)
	{
		img = "images/health3.png";
	}
	else if (self->health == 2)
	{
		img = "images/health2.png";
	}
	else if (self->health == 1)
	{
		img = "images/health1.png";
	}

	digit1 = gf2d_sprite_load_image(img);
	gf2d_sprite_draw_image(digit1, pos);

}

void player_health_display(Entity* self){
	player_health_image_set(self, vector2d(150, 25));
}
int  player_damage(Entity* self, int amount, Entity* source)
{
	//slog("OUCH");
	self->health -= amount;
	Sound *playermusic = gfc_sound_load("moozik/damage.mp3", 1, 4);
	gfc_sound_play(playermusic, 0, 0.05, -1, -1);
}
