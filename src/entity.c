#include <stdlib.h>
#include "simple_logger.h"

#include "gfc_list.h"
#include "gfc_color.h"

#include "level.h"
#include "gf2d_shape.h"
#include "camera.h"
#include "gf2d_collision.h"
#include "entity.h"

typedef struct
{
	Entity *entity_list;
	Uint32 max_entities;
}EntityManager;

static EntityManager entity_manager = { 0 };

void entity_manager_init(Uint32 max_entities)
{
	if (max_entities == 0)
	{
		slog("cannot allocate 0 entities");
		return;
	}
	if (entity_manager.entity_list != NULL)
	{
		entity_manager_free();
	}
	entity_manager.entity_list = (Entity *)gfc_allocate_array(sizeof (Entity),max_entities);
	if (entity_manager.entity_list == NULL)
	{
		slog("failed to allocate entity list");
		return;
	}
	entity_manager.max_entities = max_entities;
	atexit(entity_manager_free);
}

void entity_manager_free()
{
	if (entity_manager.entity_list != NULL)
	{
		free(entity_manager.entity_list);
	} 
	memset(&entity_manager, 0, sizeof(EntityManager));
}

void entity_update(Entity *self)
{
	if (!self)return;
	//DO ANY GENERIC UPDATE CODE
	vector2d_add(self->position, self->position, self->velocity);
	self->frame += self->frameRate;
	if (self->frame >= self->frameCount)self->frame = 0;
	//IF THERE IS A CUSTOM UPDATE, DO THAT NOW
	if (self->update)self->update(self);
}

void entity_manager_update_entities()
{
	int i;
	if (entity_manager.entity_list == NULL)
	{
		slog("entity system does not exist.");
		return;
	}
	for (i = 0; i < entity_manager.max_entities; i++)
	{
		if (entity_manager.entity_list[i]._inuse == 0)continue;
		entity_update(&entity_manager.entity_list[i]);
	}
}

void entity_manager_think_entities()
{
	int i;
	if (entity_manager.entity_list == NULL)
	{
		slog("entity system does not exist");
		return;
	}
	for (i = 0; i < entity_manager.max_entities; i++)
	{
		if (entity_manager.entity_list[i]._inuse == 0)continue;
		if (entity_manager.entity_list[i].think != NULL)
		{
			entity_manager.entity_list[i].think(&entity_manager.entity_list[i]);
		}
	}
}

void entity_manager_draw_entities()
{
	int i;
	if (entity_manager.entity_list == NULL)
	{
		slog("entity system does not exist.");
		return;
	}
	for (i = 0; i < entity_manager.max_entities; i++)
	{
		if (entity_manager.entity_list[i]._inuse == 0)continue;
		entity_draw(&entity_manager.entity_list[i]);
	}
}

Entity *entity_new()
{
	int i; 
	if (entity_manager.entity_list == NULL)
	{
		slog("entity system does not exist.");
		return NULL;
	}
	for (i = 0; i < entity_manager.max_entities; i++)
	{
		if (entity_manager.entity_list[i]._inuse)continue; //entity currenty in use
		memset(&entity_manager.entity_list[i], 0, sizeof(Entity));
		entity_manager.entity_list[i]._inuse = 1;
		return &entity_manager.entity_list[i];
	}
	slog("no free entities available.");
	return NULL;
}

void entity_free(Entity *ent)
{
	if (!ent)
	{
		slog("cannot free a NULL entity");
		return;
	}
	gf2d_sprite_free(ent->sprite);
	ent->sprite = NULL;
	ent->_inuse = 0;
}

void gf2d_entity_free_all()
{
	int i;
	for (i = 0; i < entity_manager.max_entities; i++)
	{
		if (entity_manager.entity_list[i]._inuse == 0)continue;
		entity_free(&entity_manager.entity_list[i]);
	}
}



void entity_draw(Entity *ent)
{
	Vector2D drawPosition, offset;
	if (!ent)
	{
		slog("cannot draw a NULL entity");
		return;
	}
	if (ent->draw)
	{
		ent->draw(ent);
	}
	else
	{
		if (ent->sprite == NULL)
		{
			return; //nothing to draw
		}

		offset = camera_get_offset();
		if (!camera_rect_on_screen(gfc_sdl_rect(ent->position.x, ent->position.y, ent->sprite->frame_w, ent->sprite->frame_h)))
		{
			//entity is off camera, skip
			return;
		}
		drawPosition.x = ent->position.x + offset.x;
		drawPosition.y = ent->position.y + offset.y;

		gf2d_sprite_draw(
			ent->sprite,
			//ent->position,
			drawPosition,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			(Uint32)ent->frame);

		gf2d_sprite_draw(
			ent->sprite2,
			//ent->position,
			drawPosition,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			(Uint32)ent->frame);
	}
}

void gf2d_entity_pre_sync_body(Entity* self)
{
	if (!self)return;// nothin to do
	vector2d_copy(self->body.velocity, self->velocity);
	vector2d_copy(self->body.position, self->position);
}

void gf2d_entity_post_sync_body(Entity* self)
{
	if (!self)return;// nothin to do
	//    slog("entity %li : %s old position(%f,%f) => new position (%f,%f)",self->id,self->name,self->position,self->body.position);
	vector2d_copy(self->position, self->body.position);
	vector2d_copy(self->velocity, self->body.velocity);
}
void gf2d_entity_pre_sync_all()
{
	int i;
	for (i = 0; i < entity_manager.max_entities; i++)
	{
		if (entity_manager.entity_list[i]._inuse == 0)continue;
		gf2d_entity_pre_sync_body(&entity_manager.entity_list[i]);
	}
}

void gf2d_entity_post_sync_all()
{
	int i;
	for (i = 0; i < entity_manager.max_entities; i++)
	{
		if (entity_manager.entity_list[i]._inuse == 0)continue;
		gf2d_entity_post_sync_body(&entity_manager.entity_list[i]);
	}
}

void entity_apply_gravity(Entity* self)
{
	self->velocity.y += 0.58;
	if (entity_wall_check(self, vector2d(0, 2)))
	{
		if (self->velocity.y > 0)self->velocity.y = 0;
		self->grounded = 1;
		//slog("grounded ");
		//slog(self->velocity.y);

	}
	else
	{
		self->grounded = 0;
		slog("not grounded ");
	}
}

int entity_wall_check(Entity* self, Vector2D dir)
{
	Shape s;
	int i, count;
	Collision* c;
	List* collisionList;
	CollisionFilter filter = {
		1,
		WORLD_LAYER,
		0,
		0,
		&self->body
	};

	if (!self)return 0;
	s = gf2d_body_to_shape(&self->body);
	gf2d_shape_move(&s, dir);

	collisionList = gf2d_collision_check_space_shape(level_get_space(), s, filter);
	//slog(collisionList);
	if (collisionList != NULL)
	{
		count = gfc_list_get_count(collisionList);
		for (i = 0; i < count; i++)
		{
			c = (Collision*)gfc_list_get_nth(collisionList, i);
			if (!c)continue;
			if (!c->shape)continue;
			//gf2d_shape_draw(*c->shape, gfc_color(255, 255, 0, 255), camera_get_offset());
			gf2d_shape_draw(*c->shape, gfc_color(0, 0, 0, 0), camera_get_offset());
		}
		gf2d_collision_list_free(collisionList);
		return 1;
	}
	return 0;
}

void entity_world_snap(Entity* self)
{
	if (entity_wall_check(self, vector2d(0, 0.1)))
	{
		self->position.y -= 0.1;
	}
	if (entity_wall_check(self, vector2d(0, -0.1)))
	{
		self->position.y += 0.1;
	}
	if (entity_wall_check(self, vector2d(0.1, 0)))
	{
		self->position.x -= 0.1;
	}
	if (entity_wall_check(self, vector2d(-0.1, 0)))
	{
		self->position.x += 0.1;
	}
}

List* entity_get_clipped_entities(Entity* self, Shape s, Uint32 layers, Uint32 team)
{
	CollisionFilter filter = { 0 };
	filter.ignore = &self->body;
	filter.cliplayer = layers;
	filter.team = team;
	return gf2d_collision_check_space_shape(level_get_space(), s, filter);
}

/*eol@eof*/
