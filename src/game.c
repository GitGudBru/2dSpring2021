#include <SDL.h>
#include <stdio.h>
#include <math.h>
#include "simple_logger.h"
#include "gfc_input.h"
#include "gfc_audio.h"

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
#include "box.h"
#include "mount.h"
#include "mount2.h"
#include "gf2d_space.h"
#include "gf2d_collision.h"
#include "gf2d_font.h"
#include "gf2d_mouse.h"
#include "gf2d_draw.h"
#include "gf2d_actor.h"
#include "windows_common.h"

static int _done = 0;
static int changer = 1;
static Window* _quit = NULL;
static Window* mainWin = NULL;
static Level* level = NULL;
static Entity* player = NULL;

void onCancel(void* data) //PAUSE: RETURN TO GAME
{
	_quit = NULL;
}

void onCancel2(void* data) //EXIT GAME
{
	slog("onCancel2");
	_done = 1;
	//_quit = NULL;
	mainWin = NULL;
}


void onExit2(void* data) //LEVEL 2
{
	mainWin = NULL;
	changer = 2;
	level = level_load("levels/level1.json");
	Sound *menumusic = gfc_sound_load("moozik/stage_1.mp3", 1, 1);
	gfc_sound_play(menumusic, 0, 0.05, -1, -1);
	player = player_spawn("levels/player.json");

	Entity* mount1 = mount_spawn(vector2d(520, 445));
	Entity* mount2 = mount2_spawn(vector2d(1220, 475));
	Entity* mount3 = mount3_spawn(vector2d(1920, 465));

	Entity* breakable2 = breakable_spawn(vector2d(140, 485));
	Entity* breakable3 = breakable_spawn(vector2d(180, 485));
	Entity* breakable4 = breakable_spawn(vector2d(220, 485));
	Entity* breakable5 = breakable_spawn(vector2d(260, 485));
	Entity* breakable6 = breakable_spawn(vector2d(300, 485));
	Entity* breakable7 = breakable_spawn(vector2d(340, 485));

	Entity* boss1 = boss1_spawn(vector2d(2800, 440));	
	Entity* boss2 = boss2_spawn(vector2d(3500, 480));
}

void onOK(void* data) //LEVEL 1
{
	mainWin = NULL;
	changer = 0;
	level = level_load("levels/level2.json");
	player = player_spawn("levels/player.json");
	//Entity* mount1 = mount_spawn(vector2d(220, 445));
	//Entity* mount3 = mount3_spawn(vector2d(220, 445));
	
	Entity* enemy1 = enemy1_spawn(vector2d(700, 485));
	Entity* enemy11 = enemy1_spawn(vector2d(730, 485));
	Entity* enemy12 = enemy1_spawn(vector2d(760, 485));
	Entity* enemy2 = enemy2_spawn(vector2d(1500, 485));
	Entity* enemy3 = enemy3_spawn(vector2d(1700, 485));
	Entity* enemy4 = enemy4_spawn(vector2d(1650, 485));
	Entity* enemy5 = enemy5_spawn(vector2d(2100, 485));

	/*
	if (enemy11->health <= 0)
	{
		slog("cHANGE");
		changer = 2;
	}
	*/
}

void onExit(void* data) //MENU
{
	level_clear();
	//level = NULL;	//use this instead?
	Sound *menumusic = gfc_sound_load("moozik/menu.mp3", 1, 1);
	gfc_sound_play(menumusic, 0, 0.05, -1, -1);
	mainWin = window_menu2("I <3 NJIT", onOK, onExit2, onCancel2, NULL, NULL, NULL);

	gf2d_entity_free_all();
	camera_set_position(vector2d(0, 0));
	_quit = NULL;
	changer = 1;
}


int main(int argc, char * argv[])
{
	/*variable declarations*/
	int done = 0;
	const Uint8 * keys;
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
		900,
		720,
		900,
		720,
		vector4d(0, 0, 0, 255),
		fullscreen,
		debug);

	gf2d_graphics_set_frame_delay(16);
	camera_set_position(vector2d(0, 0));

	gf2d_sprite_init(1024);
	gf2d_action_list_init(128);
	gf2d_font_init("config/font.cfg");
	gfc_input_init("config/input.cfg");
	gfc_audio_init(256, 16, 4, 1, 1, 1); 
	gf2d_windows_init(128);
	entity_manager_init(100);

	camera_set_dimensions(vector2d(900, 730));
	background = gf2d_sprite_load_image("images/backgrounds/menu2.jpg");


	SDL_ShowCursor(SDL_DISABLE);

	gf2d_mouse_load("actors/mouse.actor");	//breaks game
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
	mouse = gf2d_sprite_load_all("images/pointer.png", 32, 32, 16);

	if (changer == 1)	//MAIN MENU (?)
	{
		mainWin = window_menu2("I <3 NJIT", onOK, onExit2, onCancel2, NULL, NULL, NULL);
		Sound *menumusic = gfc_sound_load("moozik/menu.mp3", 1, 1);
		gfc_sound_play(menumusic, 0, 0.05, -1, -1);
	}
	filter.worldclip = 1;

	/*main game loop*/
	while (!_done)
	{
		gfc_input_update();
		SDL_PumpEvents();   // update SDL's internal event structures
		keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
		/*update things here*/
		SDL_GetMouseState(&mx, &my);
		mf += 0.1;
		if (mf >= 16.0)mf = 0;

		gf2d_windows_update_all();
		gf2d_mouse_update();


		if (changer != 1) 
		{
			entity_manager_think_entities();
			entity_manager_update_entities();
		}
		if (changer != 1)
		{
			level_update(level);
		}

		gf2d_space_update(space);	//maybe I can remove this
		collision = gf2d_collision_trace_space(space, vector2d(mx, my), vector2d(600, 360), filter); //maybe I can remove this
		gf2d_graphics_clear_screen();// clears drawing buffers

		gf2d_sprite_draw_image(background, vector2d(0, 0));


		if (changer != 1) 
		{
			level_draw(level);
		}

		gf2d_space_draw(space, vector2d(0, 0));		//maybe I can remove this

		if (changer != 1) 
		{
			entity_manager_draw_entities();
		}

		if (changer != 1) {
			player_health_display(player);
		}

		gf2d_windows_draw_all();
		gf2d_mouse_draw();
		gf2d_grahics_next_frame();// render current draw frame and skip to the next frame

		if ((gfc_input_command_down("exit")) && (_quit == NULL) && (mainWin == NULL))
		{
			_quit = window_yes_no("Back 2 Menu?", onExit, onCancel, NULL, NULL);
			if (player) {
				player_save(player, "levels/player_new.json");
			}
		}

		/*
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