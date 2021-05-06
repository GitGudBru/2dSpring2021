#include <SDL.h>
#include <stdio.h>
#include <math.h>
#include "simple_logger.h"
#include "gfc_input.h"

#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "entity.h"
#include "player.h"
#include "level.h"
#include "enemy1.h"
#include "enemy2.h"
#include "enemy3.h"
#include "enemy4.h"
#include "enemy5.h"
#include "boss1.h"
#include "boss2.h"
#include  "box.h"
#include "gf2d_space.h"
#include "gf2d_collision.h"
#include "gf2d_font.h"
#include "gf2d_mouse.h"
#include "gf2d_draw.h"
#include "gf2d_actor.h"
#include "windows_common.h"

static int _done = 0;
static Window* _quit = NULL;

void onCancel(void* data)
{
	_quit = NULL;
}
void onExit(void* data)
{
	_done = 1;
	_quit = NULL;

}

int main(int argc, char * argv[])
{
	/*variable declarations*/
	int done = 0;
	const Uint8 * keys;
	//Sprite *sprite;
	//Entity *ent;
	Level *level;
	Level *level2;
	int i;
	int fullscreen = 0;
	int debug = 0;
	Sprite* background = NULL;


	int mx, my;
	float mf = 0;
	Space* space = NULL;
	Collision collision;
	CollisionFilter filter = { 0 };
	static Body body[10000];// not a pointer!
	Shape shape[4];// not a pointer!    /*parse args*/
	Sprite *mouse;
	Vector4D mouseColor = { 255, 100, 255, 200 };

	for (i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "--fullscreen") == 0)
		{
			fullscreen = 1;
		}
		if (strcmp(argv[i], "--debug") == 0)
		{
			debug = 1;
		}
	}


	/*program initializtion*/
	init_logger("gf2d.log");
	slog("---==== BEGIN ====---");
	gf2d_graphics_initialize(
		"gf2d",
		1200,
		720,
		1200,
		720,
		vector4d(0, 0, 0, 255),
		fullscreen,
		debug);       
	gf2d_graphics_set_frame_delay(16);
	//camera_set_dimensions(vector2d(920, 720));
	camera_set_position(vector2d(0, 0));

	gf2d_sprite_init(1024);
	gf2d_action_list_init(128);
	gf2d_font_init("config/font.cfg");
	//gfc_input_init("config/input.json");
	gf2d_windows_init(128);
	entity_manager_init(100);
	//level = level_new();
	camera_set_dimensions(vector2d(1200, 720));
	//background = gf2d_sprite_load_image("images/backgrounds/bg_flat.png");


	SDL_ShowCursor(SDL_DISABLE);

	//gf2d_mouse_load("actors/mouse.actor");	//breaks game
	mf = 0;

	space = gf2d_space_new_full(
		3,
		gf2d_rect(0, 0, 1200, 700),
		//gf2d_rect(0, 0, 3936, 700),
		0.1,
		vector2d(0, 0.1),
		1,
		1);

	/*demo setup*/
	//mouse = gf2d_sprite_load_all("images/pointer.png", 32, 32, 16);
	//level = level_load("levels/level1.json");
	level = level_load("levels/level2.json");
	Entity* player = player_spawn("levels/player.json");
	Entity* breakable = breakable_spawn(vector2d(100, 485)); //near
	//Entity* breakable2 = breakable_spawn(vector2d(300, 680)); //up
	Entity* breakable2 = breakable_spawn(vector2d(140, 485));
	Entity* breakable3 = breakable_spawn(vector2d(180, 485)); //up
	Entity* breakable4 = breakable_spawn(vector2d(220, 485)); //up
	Entity* breakable5 = breakable_spawn(vector2d(260, 485)); //up
	Entity* breakable6 = breakable_spawn(vector2d(300, 485)); //up
	Entity* breakable7 = breakable_spawn(vector2d(340, 485)); //up
	/*
	Entity* enemy1 = enemy1_spawn(vector2d(700, 485));
	Entity* enemy11 = enemy1_spawn(vector2d(730, 485));
	Entity* enemy12 = enemy1_spawn(vector2d(760, 485));
	Entity* enemy2 = enemy2_spawn(vector2d(1500, 485));
	Entity* enemy3 = enemy3_spawn(vector2d(1700, 485));
	Entity* enemy4 = enemy4_spawn(vector2d(1650, 485));
	Entity* enemy5 = enemy5_spawn(vector2d(2100, 485));
	Entity* boss1 = boss1_spawn(vector2d(2800, 440));	//SPAWN THE BOSS HIGHER OFF THE GROUND
	Entity* boss2 = boss2_spawn(vector2d(3500, 480));
	level_add_entity(player);
	level_add_entity(breakable);
	level_add_entity(breakable2);
	level_add_entity(breakable3);
	level_add_entity(breakable4);
	level_add_entity(breakable5);
	level_add_entity(breakable6);
	level_add_entity(breakable7);
	level_add_entity(enemy1);
	level_add_entity(enemy11);
	level_add_entity(enemy12);
	level_add_entity(enemy2);
	level_add_entity(enemy3);
	level_add_entity(enemy4);
	level_add_entity(enemy5);
	level_add_entity(boss1);
	level_add_entity(boss2);
	*/
	level_add_entity(player);
	Entity* enemy3 = enemy3_spawn(vector2d(700, 485));
	level_add_entity(enemy3);

	Entity* boss2 = boss2_spawn(vector2d(700, 480));
	level_add_entity(boss2);

	filter.worldclip = 1;

	/*main game loop*/
	while (!_done)
	{
		/*
		if (boss1->health <= 1)
		{
		slog("switch lvl");
		//level_free(level);
		level2 = level_load("levels/level2.json");
		Entity* boss2 = boss2_spawn(vector2d(700, 900));
		level_add_entity(boss2);
		level_update(level2);
		level_draw(level2);
		}
		*/
		gfc_input_update();
		SDL_PumpEvents();   // update SDL's internal event structures
		keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
		/*update things here*/
		SDL_GetMouseState(&mx, &my);
		mf += 0.1;
		if (mf >= 16.0)mf = 0;

		gf2d_windows_update_all();

		gf2d_mouse_update();


		entity_manager_update_entities();

		level_update(level);

		gf2d_space_update(space);

		collision = gf2d_collision_trace_space(space, vector2d(mx, my), vector2d(600, 360), filter);

		gf2d_graphics_clear_screen();// clears drawing buffers

		gf2d_sprite_draw_image(background, vector2d(0, 0));

		level_draw(level);


		gf2d_space_draw(space, vector2d(0, 0));

		entity_manager_think_entities();
		entity_manager_draw_entities();

		//UI elements last
		/*
		gf2d_sprite_draw(
			mouse,
			vector2d(mx, my),
			NULL,
			NULL,
			NULL,
			NULL,
			&mouseColor,
			(int)mf);
			*/
		player_health_display(player);

		//gf2d_font_draw_line_tag("Press F4 to quit!", FT_H1, gfc_color(255, 255, 255, 255), vector2d(0, 0));
		gf2d_windows_draw_all();
		gf2d_mouse_draw();

		gf2d_grahics_next_frame();// render current draw frame and skip to the next frame

		if (keys[SDL_SCANCODE_ESCAPE])
		{
			//slog("exit!");
			_quit = window_yes_no("Exit?", onExit, onCancel, NULL, NULL);
			//player_save(player, "levels/player_new.json");
		}

		/*
		if ((gfc_input_command_down("exit")))
		{
			slog("exit!");
			_quit = window_yes_no("Exit?", onExit, onCancel, NULL, NULL);
			player_save(player, "levels/player_new.json");
		}

		
		if (keys[SDL_SCANCODE_ESCAPE])
		{
			player_save(player, "levels/player_new.json");
			done = 1; // exit condition
		}
		
		//if (player->health <= 0)
		//{
		//	player_save(player, "levels/player_new.json");
		//	done = 1; // exit condition
		//}

		if (boss2->health <= 0)
		{
			player_save(player, "levels/player_new.json");
			done = 1; // exit condition
		}
		*/
		//slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
	}
	slog("---==== END ====---");
	return 0;
}
/*eol@eof*/