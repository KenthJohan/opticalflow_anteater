#include "mainer.h"
#include "module_weldvisi.h"

static char * a = "2022-12-15_14-56-29.mp4";

void mainer(ecs_world_t * world)
{
    ECS_IMPORT(world, SimpleModule);

	//https://www.flecs.dev/explorer/?remote=true
    ecs_singleton_set(world, EcsRest, {0});

	ecs_entity_t cam = ecs_new_entity(world, "Camera1");
	ecs_set(world, cam, Device, {a});
	ecs_add(world, cam, Camera);
    ecs_add_pair(world, cam, Action, Open);

	{
		ecs_entity_t e = ecs_new_entity(world, "View1");
		ecs_set(world, e, Weldvisi_View, {10});
		ecs_set_pair(world, e, Vec2i32, CropPosition, {0, 0});
		ecs_set_pair(world, e, Vec2i32, CropSize, {100, 100});
		ecs_add_pair(world, e, Uses, cam);
	}

	{
		ecs_entity_t e = ecs_new_entity(world, "View2");
		ecs_set(world, e, Weldvisi_View, {10});
		ecs_set_pair(world, e, Vec2i32, CropPosition, {100, 0});
		ecs_set_pair(world, e, Vec2i32, CropSize, {100, 100});
		ecs_add_pair(world, e, Uses, cam);
	}
}