#ifndef __CAMERA_H__
#define __CAMERA_H__

<<<<<<< HEAD
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
* @brief check if a point is on screen
*/
Bool camera_point_on_screen(Vector2D point);

/**
* @brief check if the rectangle is on screen
*/
Bool camera_rect_on_screen(SDL_Rect rect);

/**
* @brief return the rectangle that describes the camera;
* @return the rectangle
* @note hehe
*/
SDL_Rect camera_get_rect();

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

/**
* @brief get the camera's current dimenstions (extent)
* @return a vector containing the width and height of the camera
*/
Vector2D camera_get_dimensions();

#endif
=======
#include <SDL.h>

#include "gfc_vector.h"

#include "gf2d_shape.h"

/**
 * @brief set the position and size of the screen camera
 * @param x the current left position of the camera
 * @param y the current top position of the camera
 * @param w the width of the camera
 * @param h the height of the camera
 */
void camera_set_dimensions(Sint32 x,Sint32 y,Uint32 w,Uint32 h);

/**
 * @brief get the position and size of the camera
 * @return the camera view
 */
Rect camera_get_dimensions();

/**
 * @brief get the current camera position
 * @return the x,y position of the camera
 */

Vector2D camera_get_position();

/**
 * @brief limit the position of the camera by these bounds
 * @param x the minimum x position for the camera
 * @param y the minimum y position for the camera
 * @param w the right bounds
 * @param w the bottom bounds
 */
void camera_set_bounds(Sint32 x,Sint32 y,Uint32 w,Uint32 h);

/**
 * @brief get the current camera bounds
 * @return the camera bounding rect
 */
Rect camera_get_bounds();

/**
 * @brief move the camera, but limit by bounds
 * @param v the vector to move the camera by
 */
void camera_move(Vector2D v);

/**
 * @brief move the camera to new position, but respect bounds
 * @param position the ideal new top left position of the camera
 */
void camera_set_position(Vector2D position);

/**
 * @brief move the camera to ne position, but do not respect bounds
 * @param position the new top left position of the camera
 */
void camera_set_position_absolute(Vector2D position);
#endif
>>>>>>> aa8cfc5270abbf06d47e74542bb51614c099b858
