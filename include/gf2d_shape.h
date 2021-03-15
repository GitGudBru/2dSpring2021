#ifndef __GF2D_SHAPE_H__
#define __GF2D_SHAPE_H__

#include <SDL.h>
#include "gfc_vector.h"
#include "gfc_color.h"


typedef struct
{
	double x1, y1, x2, y2;
}Edge;

typedef struct
{
	double x, y, w, h;
}Rect;

typedef struct
{
	double       x, y, r;
}Circle;

typedef enum
{
	ST_RECT,
	ST_CIRCLE,
	ST_EDGE
}ShapeTypes;

typedef struct
{
	ShapeTypes type;
	union
	{
		Circle c;
		Rect r;
		Edge e;
	}s;
}Shape;

/**
* @brief convert an SDL Rect to a GF2D rect
* @param r the SDL Rect to convert
* @return a GF2D rect
*/
Rect gf2d_rect_from_sdl_rect(SDL_Rect r);

/**
* @brief convert a GF2D rect to an SDL rect
* @param r the GF2D rect to convert
* @return an SDL rect
*/
SDL_Rect gf2d_rect_to_sdl_rect(Rect r);

//Rect shape_rect_from_Vector4d(Vector4D v);

/**
* @brief check if two circles are colliding
* @param a circle A
* @param b circle B
* @param returns true is there is overlap, false otherwise
*/
Uint8 gf2d_circle_overlap(Circle a, Circle b);

/**
* @brief check if the point lies within the circle c
* @param p the point to check
* @param c the circle to check
* @return true if the point is in the circle, false otherwise
*/
Uint8 gf2d_point_in_cicle(Vector2D p, Circle c);

/**
* @brief check if a point lies within a rectangle
* @param p the point to check
* @param r the rectangle to check
* @return true if the point is inside the rectangle, false otherwise
*/
Uint8 gf2d_point_in_rect(Vector2D p, Rect r); //maybe Bool instead of Uint8

/**
* @brief check if two rectangles are colliding
* @param a rect A
* @param b rect B
* @return true if there is any overlap, false otherwise
*/
Uint8 gf2d_rect_overlap(Rect a, Rect b); //gf2d_rect_collision

#endif