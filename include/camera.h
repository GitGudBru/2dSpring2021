#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "gfc_types.h"
#include "gfc_vector.h"

/**
* @brief Set the size of the camera in width and height
* @param size the width and height of the camera
*/
void camera_set_dimensions(Vector2D size);


/**
* @brief set the camera position 
* @param position the vector to set the camera 
*/
void camera_set_position(Vector2D position);

/**
* @brief move the camera relative to its current position
* @param move the ammount to move the camera by
*/
void camera_move(Vector2D move);

/**
* @brief check if the rectangle is on screen
*/
Bool camera_rect_on_screen(SDL_Rect rect);

/**
* @brief return the offset for things to be drawn relative to the camera
* @return the inverse of the camera coordinates
*/
Vector2D camera_get_offset();

/**
* @brief get the camera's world position
* @return the camera's position in world coordinates
*/
Vector2D camera_get_position();


#endif