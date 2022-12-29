#include "mainer.h"
#include "EgMotion.h"
#include "EgMemory.h"
#include "EgVideo.h"
#include "EgTypes.h"
#include "EgDraws.h"



void mainer(ecs_world_t * world)
{
	ecs_log_set_level(1);
    ECS_IMPORT(world, EgMotion);
    ECS_IMPORT(world, EgMemory);
    ECS_IMPORT(world, EgVideo);
    ECS_IMPORT(world, EgTypes);
    ECS_IMPORT(world, EgDraws);

	//https://www.flecs.dev/explorer/?remote=true
    ecs_singleton_set(world, EcsRest, {0});



	


	ecs_entity_t cam = ecs_new_entity(world, "Camera1");
	ecs_set(world, cam, Device, {"2022-12-15_14-56-29.mp4"});
	ecs_add(world, cam, Camera);
	ecs_add(world, cam, Memory);
	ecs_add(world, cam, Window);
	ecs_set(world, cam, Vec2i32, {0, 0});
	ecs_set_pair(world, cam, Vec2i32, Resolution, {0, 0});
    ecs_add_pair(world, cam, Action, Open);


	{
		ecs_entity_t show = ecs_new_entity(world, "Show");
		ecs_add(world, show, Window);
		ecs_add(world, show, Memory);
		ecs_set_pair(world, show, Vec2i32, Resolution, {0, 0});
	}



	{
		ecs_entity_t e = ecs_new_entity(world, "View1");
		ecs_set(world, e, Weldvisi_View, {10});
		ecs_set_pair(world, e, Vec2i32, CropPosition, {0, 0});
		ecs_set_pair(world, e, Vec2i32, CropSize, {100, 100});
		ecs_set_pair(world, e, Vec2f32, Velocity, {0, 0});
		ecs_add_pair(world, e, Uses, cam);
	}

	{
		ecs_entity_t e = ecs_new_entity(world, "View2");
		ecs_set(world, e, Weldvisi_View, {10});
		ecs_set_pair(world, e, Vec2i32, CropPosition, {100, 0});
		ecs_set_pair(world, e, Vec2i32, CropSize, {100, 100});
		ecs_set_pair(world, e, Vec2f32, Velocity, {0, 0});
		ecs_add_pair(world, e, Uses, cam);
	}


}