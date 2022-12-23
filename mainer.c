#include "mainer.h"
#include "module_weldvisi.h"

void mainer(ecs_world_t * world)
{
    ECS_IMPORT(world, SimpleModule);

	ecs_entity_t e1 = ecs_new_entity(world, "View1");
	ecs_set(world, e1, Weldvisi_View, {10});
	ecs_set_pair(world, e1, Vec2i32, CropPosition, {0, 0});
	ecs_set_pair(world, e1, Vec2i32, CropSize, {100, 100});

	ecs_entity_t e2 = ecs_new_entity(world, "View2");
	ecs_set(world, e2, Weldvisi_View, {10});
	ecs_set_pair(world, e2, Vec2i32, CropPosition, {100, 0});
	ecs_set_pair(world, e2, Vec2i32, CropSize, {100, 100});
}