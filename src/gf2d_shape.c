#include "simple_logger.h"
#include "gf2d_draw.h"
#include "gf2d_shape.h"

Circle gf2d_circle(float x, float y, float r)
{
	Circle c;
	c.x = x;
	c.y = y;
	c.r = r;
	return c;
}

SDL_Rect gf2d_rect_to_sdl_rect(Rect r)
{
	SDL_Rect r2;
	r2.x = r.x;
	r2.y = r.y;
	r2.w = r.w;
	r2.h = r.h;
	return r2;
}
//Rect shape_rect_from_Vector4d(Vector4D v);

/**
* @brief check if two circles are colliding
* @param a circle A
* @param b circle B
* @param returns true is there is overlap, false otherwise
*/
Uint8 gf2d_circle_overlap(Circle a, Circle b) 
{
	float radii2;
	float c2;
	radii2 = (a.r*a.r) + (b.r * b.r);
	c2 = ((a.x - b.x) * (a.x - b.x)) + ((a.y - b.y) * (a.y - b.y));
	if (radii2 <= c2)return 0;
	return 1;
	//DJ's CODE, NOT GF2D
}


Uint8 gf2d_point_in_cicle(Vector2D p, Circle c)
{
	if(gf2d_circle_overlap(gf2d_circle(p.x, p.y, 0), c)<=0)return 1;
	return 0;
	//DJ's CODE, NOT GF2D
}


Uint8 gf2d_point_in_rect(Vector2D p, Rect r) //maybe Bool instead of Uint8
{
	if ((p.x >= r.x) && (p.x <= r.x + r.w) &&
		(p.y >= r.y) && (p.y <= r.y + r.h))
	{
		return 1;
	}	
	return 0;
}

Uint8 gf2d_rect_overlap(Rect a, Rect b) //gf2d_rect_collision
{
	//return gf2d_rect_overlap_poc(a,b,NULL,NULL);
	if ((a.x > b.x + b.w) ||
		(b.x > a.x + a.w) ||
		(a.y > b.y + b.h) ||
		(b.y > a.y + a.h))
	{
		return 0;
	}
	return 1;
}
/*
Uint8 gf2d_rect_overlap_poc(Rect a, Rect b, Vector2D *poc, Vector2D *normal)
{
	if ((a.x > b.x + b.w) ||
		(b.x > a.x + a.w) ||
		(a.y > b.y + b.h) ||
		(b.y > a.y + a.h))
	{
		return 0;
	}
	if (poc)
	{
		poc->y = poc->x = 0;
		if (normal)normal->x = normal->y = 0;
		if (a.x + 1 >= b.x + b.w)
		{
			poc->x = a.x;
			if (normal)normal->x = -1;
		}
		else if (b.x + 1 >= a.x + a.w)
		{
			poc->x = b.x;
			if (normal)normal->x = 1;
		}
		if (a.y + 1 >= b.y + b.h)
		{
			poc->y = a.y;
			if (normal)normal->y = -1;
		}
		if (b.y + 1 >= a.y + a.h)
		{
			if (normal)normal->y = 1;
			poc->y = b.y;
		}
	}
	return 1;
}
*/
/*eol@eof*/
