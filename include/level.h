#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "gfc_types.h"
#include "gfc_text.h"

#include "gf2d_sprite.h"

typedef enum
{
	TT_Empty,
	TT_Brick,
	TT_Max
}TileTypes;


typedef struct
{
	Sprite *bgImage;		/**<background image for level*/
	Sprite *tileSet;		/**<sprite for tileset*/
	TileTypes *tileMap;     /**<Tiles for the level*/
	Uint32 levelWidth;		/**<how many tiles per row the levels are*/
	Uint32 levelHeight;		/**<how many tiles per column the levels are*/
	int	   tileWidth;		/**<how wide the tiles are (pixel)*/
	int    tileHeight;		/**<how high the tiles are (pixel)*/
	int	   tileFPL;
}Level;

/**
* @brief allocate and initialize a level
* @return NULL on error, else a pointer to a new level
*/
Level *level_new();

/**
* @brief load a level from a json file
* @param filename the file to load
* @return NULL on error (See the logs) a loaded level otherwise
*/
Level *level_load(const char *filename);

/**
* @brief free up a previously loaded level
* @param level a pointer to the level to free
*/
void level_free(Level *level);

/**
* @brief draw a level
* @param level the level the draw
*/
void level_draw(Level *level);




#endif