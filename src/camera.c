<<<<<<< HEAD
#include <stdlib.h>
#include "simple_logger.h"
#include "gf2d_shape.h"
#include "camera.h"

static SDL_Rect Camera = { 0 };

void camera_set_dimensions(Vector2D size)
{
	Camera.w = size.x;
	Camera.h = size.y;
}

void camera_set_position(Vector2D position)
{
	Camera.x = (int)position.x;
	Camera.y = (int)position.y;
}

void camera_move(Vector2D move)
{
	Camera.x += move.x;
	Camera.y += move.y;
}

Vector2D camera_get_offset()
{
	Vector2D position;
	position.x = -Camera.x;
	position.y = -Camera.y;
	return position;
}

Bool camera_point_on_screen(Vector2D point)
{
	if ((point.x < Camera.x) || (point.x > Camera.x + Camera.w) ||
		(point.y < Camera.y) || (point.y > Camera.y + Camera.h))
	{
		return false;
	}
	return true;
}

Bool camera_rect_on_screen(SDL_Rect rect)
{
	if (((rect.x + rect.w) < Camera.x) || (rect.x > (Camera.x + Camera.w)) ||
		((rect.y + rect.h) < Camera.y) || (rect.y > (Camera.y + Camera.h)))
	{
		return false;
	}
	return true;
}

SDL_Rect camera_get_rect()
{
	return Camera;
}

Vector2D camera_get_position()
{
	Vector2D position;
	position.x = Camera.x;
	position.y = Camera.y;
	return position;
}

Vector2D camera_get_dimensions()
{
	Vector2D size;
	size.x = Camera.w;
	size.y = Camera.h;
	return size;
}
=======
#include "camera.h"
#include "simple_logger.h"

typedef struct
{
    Rect view;
    Rect bounds;
}Camera;
static Camera _camera = {0};

void camera_set_dimensions(Sint32 x,Sint32 y,Uint32 w,Uint32 h)
{
    gf2d_rect_set(_camera.view,x,y,w,h);
}

Rect camera_get_bounds()
{
    return _camera.bounds;
}

Rect camera_get_dimensions()
{
    return _camera.view;
}

Vector2D camera_get_position()
{
    return vector2d(_camera.view.x,_camera.view.y);
}

void camera_set_bounds(Sint32 x,Sint32 y,Uint32 w,Uint32 h)
{
    gf2d_rect_set(_camera.bounds,x,y,w,h);
}

void camera_bind()
{
    if (_camera.view.x < _camera.bounds.x)_camera.view.x = _camera.bounds.x;
    if (_camera.view.x + _camera.view.w > _camera.bounds.x + _camera.bounds.w)_camera.view.x = _camera.bounds.x + _camera.bounds.w - _camera.view.w;
    if (_camera.view.y < _camera.bounds.y)_camera.view.y = _camera.bounds.y;
    if (_camera.view.y + _camera.view.h > _camera.bounds.y + _camera.bounds.h)_camera.view.y = _camera.bounds.y + _camera.bounds.h - _camera.view.h;
}

void camera_move(Vector2D v)
{
    vector2d_add(_camera.view,v,_camera.view);
    camera_bind();
}

void camera_set_position(Vector2D position)
{
    vector2d_copy(_camera.view,position);
    camera_bind();
}

void camera_set_position_absolute(Vector2D position)
{
    vector2d_copy(_camera.view,position);
}

/*eol@eof*/
>>>>>>> aa8cfc5270abbf06d47e74542bb51614c099b858
