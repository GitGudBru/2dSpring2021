#include <SDL.h>
#include <math.h>
#include "simple_logger.h"
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "entity.h"
#include "player.h"
#include "level.h"

int main(int argc, char * argv[])
{
    /*variable declarations*/
    int done = 0;
    const Uint8 * keys;
    //Sprite *sprite;
	//Entity *ent;
	Level *level;
    
    int mx,my;
    float mf = 0;
    Sprite *mouse;
    Vector4D mouseColor = {255,100,255,200};
    
    /*program initializtion*/
    init_logger("gf2d.log");
    slog("---==== BEGIN ====---");
    gf2d_graphics_initialize(
        "gf2d",
        1200,
        720,
        1200,
        720,
        vector4d(0,0,0,255),
        0);
    gf2d_graphics_set_frame_delay(16);
	camera_set_dimensions(vector2d(1200, 7200));
	camera_set_position(vector2d(0, 0));

    gf2d_sprite_init(1024);
	entity_manager_init(100);
	//level = level_new();


    SDL_ShowCursor(SDL_DISABLE);
    
    /*demo setup*/
    //sprite = gf2d_sprite_load_image("images/backgrounds/bg_flat.png");
    mouse = gf2d_sprite_load_all("images/pointer.png",32,32,16);
	level = level_load("levels/level1.json");
	player_spawn(vector2d(100,600));

	//ent = entity_new();
	//ent->sprite = gf2d_sprite_load_all("images/ed210_top.png", 128, 128, 16);
	//ent->frameRate = 0.1;
	//ent->frameCount = 16;

    /*main game loop*/
    while(!done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        /*update things here*/
        SDL_GetMouseState(&mx,&my);
        mf+=0.1;
        if (mf >= 16.0)mf = 0;
        
		//ent->frame+= 0.1;
		//if (ent->frame >= 16)ent->frame = 0; //update animation

		//ent->position.x++;
		//ent->position.y++; //update position

		entity_manager_update_entities();
        
        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
            //gf2d_sprite_draw_image(sprite,vector2d(0,0));
			level_draw(level);

			//entity_draw(ent);
			entity_manager_draw_entities();

            //UI elements last
            gf2d_sprite_draw(
                mouse,
                vector2d(mx,my),
                NULL,
                NULL,
                NULL,
                NULL,
                &mouseColor,
                (int)mf);
        gf2d_grahics_next_frame();// render current draw frame and skip to the next frame

		if (keys[SDL_SCANCODE_D]) {
			camera_move(vector2d(1, 0));
		}
		if (keys[SDL_SCANCODE_A]) {
			camera_move(vector2d(-1, 0));
		}
		if (keys[SDL_SCANCODE_UP]) {
			camera_move(vector2d(0, -10));
		}
		if (keys[SDL_SCANCODE_DOWN]) {
			camera_move(vector2d(0, 10));
		}
        
        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
        //slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }

	//entity_free(ent);

    slog("---==== END ====---");
    return 0;
}
/*eol@eof*/
