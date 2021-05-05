#include <stdlib.h>
#include "simple_logger.h"

#include "gf2d_dynamic_body.h"
#include "gf2d_draw.h"

#include "gf2d_collision.h"

Collision *gf2d_collision_new()
{
<<<<<<< HEAD
	Collision *collision = NULL;
	collision = (Collision *)malloc(sizeof(Collision));
	if (!collision)
	{
		slog("failed to allocate data for a collision object");
		return NULL;
	}
	memset(collision, 0, sizeof(Collision));
	return collision;
=======
    Collision *collision = NULL;
    collision = (Collision *)malloc(sizeof(Collision));
    if (!collision)
    {
        slog("failed to allocate data for a collision object");
        return NULL;
    }
    memset(collision,0,sizeof(Collision));
    return collision;
>>>>>>> aa8cfc5270abbf06d47e74542bb51614c099b858
}

void gf2d_collision_free(Collision *collision)
{
<<<<<<< HEAD
	if (!collision)return;
	free(collision);
=======
    if (!collision)return;
    free(collision);
>>>>>>> aa8cfc5270abbf06d47e74542bb51614c099b858
}

void gf2d_collision_list_clear(List *list)
{
<<<<<<< HEAD
	int i, count;
	Collision *collision;
	if (!list)return;
	count = gfc_list_get_count(list);
	for (i = 0; i < count; i++)
	{
		collision = (Collision*)gfc_list_get_nth(list, i);
		if (!collision)continue;
		gf2d_collision_free(collision);
	}
	for (i = 0; i < count; i++)
	{
		gfc_list_delete_last(list);
	}
=======
    int i, count;
    Collision *collision;
    if (!list)return;
    count = gfc_list_get_count(list);
    for (i = 0; i < count;i++)
    {
        collision = (Collision*)gfc_list_get_nth(list,i);
        if (!collision)continue;
        gf2d_collision_free(collision);
    }
    for (i = 0; i < count;i++)
    {
        gfc_list_delete_last(list);
    }
>>>>>>> aa8cfc5270abbf06d47e74542bb51614c099b858
}

void gf2d_collision_list_free(List *list)
{
<<<<<<< HEAD
	int i, count;
	Collision *collision;
	if (!list)return;
	count = gfc_list_get_count(list);
	for (i = 0; i < count; i++)
	{
		collision = (Collision*)gfc_list_get_nth(list, i);
		if (!collision)continue;
		gf2d_collision_free(collision);
	}
	gfc_list_delete(list);
=======
    int i, count;
    Collision *collision;
    if (!list)return;
    count = gfc_list_get_count(list);
    for (i = 0; i < count;i++)
    {
        collision = (Collision*)gfc_list_get_nth(list,i);
        if (!collision)continue;
        gf2d_collision_free(collision);
    }
    gfc_list_delete(list);
>>>>>>> aa8cfc5270abbf06d47e74542bb51614c099b858
}

Collision *gf2d_collision_space_static_shape_clip(Shape a, Shape *s)
{
<<<<<<< HEAD
	Collision *collision;
	Vector2D poc, normal;
	if (!gf2d_shape_overlap_poc(a, *s, &poc, &normal))
	{
		return NULL;
	}
	collision = gf2d_collision_new();
	collision->collided = 1;
	collision->blocked = 1;
	vector2d_copy(collision->pointOfContact, poc);
	vector2d_copy(collision->normal, normal);
	collision->shape = s;
	collision->body = NULL;
	collision->bounds = 0;
	collision->timeStep = 0;
	return collision;
=======
    Collision *collision;
    Vector2D poc,normal;
    if (!gf2d_shape_overlap_poc(a, *s, &poc, &normal))
    {
        return NULL;
    }
    collision = gf2d_collision_new();
    collision->collided = 1;
    collision->blocked = 1;
    vector2d_copy(collision->pointOfContact,poc);
    vector2d_copy(collision->normal,normal);
    collision->shape = s;
    collision->body = NULL;
    collision->bounds = 0;
    collision->timeStep = 0; 
    return collision;
>>>>>>> aa8cfc5270abbf06d47e74542bb51614c099b858
}

Collision *gf2d_collision_space_dynamic_body_clip(Shape a, DynamicBody *d)
{
<<<<<<< HEAD
	Shape s;
	Collision *collision;
	Vector2D poc, normal;
	if (!d)return NULL;
	s = gf2d_dynamic_body_to_shape(d);
	if (!gf2d_shape_overlap_poc(a, s, &poc, &normal))
	{
		return NULL;
	}
	collision = gf2d_collision_new();
	collision->collided = 1;
	collision->blocked = 1;
	vector2d_copy(collision->pointOfContact, poc);
	vector2d_copy(collision->normal, normal);
	collision->shape = d->body->shape;
	collision->body = d->body;
	collision->bounds = 0;
	collision->timeStep = 0;
	return collision;
}


List *gf2d_collision_check_space_shape(Space *space, Shape shape, CollisionFilter filter)
{
	int i, count;
	Shape *staticShape;
	DynamicBody *db;
	Collision *collision;
	List *collisionList = NULL;
	collisionList = gfc_list_new();
	if (filter.worldclip)
	{
		count = gfc_list_get_count(space->staticShapes);
		for (i = 0; i < count; i++)
		{
			staticShape = (Shape*)gfc_list_get_nth(space->staticShapes, i);
			if (!staticShape)continue;
			// check for layer compatibility
			collision = gf2d_collision_space_static_shape_clip(shape, staticShape);
			if (collision == NULL)continue;
			collisionList = gfc_list_append(collisionList, (void*)collision);
		}
		//check if the shape clips the level bounds
		/*        collision = gf2d_dynamic_body_bounds_collision_check(db,space->bounds,t);
		if (collision != NULL)
		{
		db->collisionList = gfc_list_append(db->collisionList,(void*)collision);
		}*/
	}
	if (filter.cliplayer)
	{
		count = gfc_list_get_count(space->dynamicBodyList);
		for (i = 0; i < count; i++)
		{
			db = (DynamicBody*)gfc_list_get_nth(space->dynamicBodyList, i);
			if (!db)continue;
			if (db->body == filter.ignore)continue;
			if (!(filter.cliplayer & db->body->cliplayer))continue;
			// check for layer compatibility
			collision = gf2d_collision_space_dynamic_body_clip(shape, db);
			if (collision == NULL)continue;
			collisionList = gfc_list_append(collisionList, (void*)collision);
		}

	}

	return collisionList;
}

Collision gf2d_collision_trace_space(Space *space, Vector2D start, Vector2D end, CollisionFilter filter)
{
	Collision out = { 0 };
	Collision *collision = NULL;
	Collision *best = NULL;
	double     bestDistance = -1;
	double     distance;
	double     length;
	int count, i;
	List *collisionList;
	collisionList = gf2d_collision_check_space_shape(space, gf2d_shape_from_edge(gf2d_edge_from_vectors(start, end)), filter);
	if (!collisionList)
	{
		return out;
	}
	count = gfc_list_get_count(collisionList);
	for (i = 0; i < count; i++)
	{
		collision = (Collision*)gfc_list_get_nth(collisionList, i);
		if (!collision)continue;
		if (!best)
		{
			best = collision;
			bestDistance = vector2d_magnitude_between(start, collision->pointOfContact);
			continue;
		}
		distance = vector2d_magnitude_between(start, collision->pointOfContact);
		if (distance < bestDistance)
		{
			best = collision;
			bestDistance = distance;
		}
	}
	if (best != NULL)
	{
		length = vector2d_magnitude_between(start, end);
		if (!length)
		{
			best->timeStep = 0;
		}
		else
		{
			best->timeStep = bestDistance / length;
		}
		memcpy(&out, best, sizeof(Collision));
	}
	gf2d_collision_list_free(collisionList);
	return out;
}


/*eol@eof*/
=======
    Shape s;
    Collision *collision;
    Vector2D poc,normal;
    if (!d)return NULL;
    s = gf2d_dynamic_body_to_shape(d);
    if (!gf2d_shape_overlap_poc(a, s, &poc, &normal))
    {
        return NULL;
    }
    collision = gf2d_collision_new();
    collision->collided = 1;
    collision->blocked = 1;
    vector2d_copy(collision->pointOfContact,poc);
    vector2d_copy(collision->normal,normal);
    collision->shape = d->body->shape;
    collision->body = d->body;
    collision->bounds = 0;
    collision->timeStep = 0; 
    return collision;
}


List *gf2d_collision_check_space_shape(Space *space, Shape shape,CollisionFilter filter)
{
    int i,count;
    Shape *staticShape;
    DynamicBody *db;
    Collision *collision;
    List *collisionList = NULL;
    collisionList = gfc_list_new();
    if (filter.worldclip)
    {
        count = gfc_list_get_count(space->staticShapes);
        for (i = 0; i < count;i++)
        {
            staticShape = (Shape*)gfc_list_get_nth(space->staticShapes,i);
            if (!staticShape)continue;
            // check for layer compatibility
            collision = gf2d_collision_space_static_shape_clip(shape, staticShape);
            if (collision == NULL)continue;
            collisionList = gfc_list_append(collisionList,(void*)collision);
        }
        //check if the shape clips the level bounds
/*        collision = gf2d_dynamic_body_bounds_collision_check(db,space->bounds,t);
        if (collision != NULL)
        {
            db->collisionList = gfc_list_append(db->collisionList,(void*)collision);
        }*/
    }
    if (filter.cliplayer)
    {
        count = gfc_list_get_count(space->dynamicBodyList);
        for (i = 0; i < count;i++)
        {
            db = (DynamicBody*)gfc_list_get_nth(space->dynamicBodyList,i);
            if (!db)continue;
            if (db->body == filter.ignore)continue;
            if (!(filter.cliplayer & db->body->cliplayer))continue;
            // check for layer compatibility
            collision = gf2d_collision_space_dynamic_body_clip(shape, db);
            if (collision == NULL)continue;
            collisionList = gfc_list_append(collisionList,(void*)collision);
        }

    }

    return collisionList;
}

Collision gf2d_collision_trace_space(Space *space, Vector2D start, Vector2D end ,CollisionFilter filter)
{
    Collision out = {0};
    Collision *collision = NULL;
    Collision *best = NULL;
    double     bestDistance = -1;
    double     distance;
    double     length;
    int count,i;
    List *collisionList;
    collisionList = gf2d_collision_check_space_shape(space, gf2d_shape_from_edge(gf2d_edge_from_vectors(start,end)),filter);
    if (!collisionList)
    {
        return out;
    }
    count = gfc_list_get_count(collisionList);
    for (i =0; i < count;i++)
    {
        collision = (Collision*)gfc_list_get_nth(collisionList,i);
        if (!collision)continue;
        if (!best)
        {
            best = collision;
            bestDistance = vector2d_magnitude_between(start,collision->pointOfContact);
            continue;
        }
        distance = vector2d_magnitude_between(start,collision->pointOfContact);
        if (distance < bestDistance)
        {
            best = collision;
            bestDistance = distance;
        }
    }
    if (best != NULL)
    {
        length = vector2d_magnitude_between(start,end);
        if (!length)
        {
            best->timeStep = 0;
        }
        else
        {
            best->timeStep = bestDistance / length;
        }
        memcpy(&out,best,sizeof(Collision));
    }
    gf2d_collision_list_free(collisionList);
    return out;
}


/*eol@eof*/
>>>>>>> aa8cfc5270abbf06d47e74542bb51614c099b858
