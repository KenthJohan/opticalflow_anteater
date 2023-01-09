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
	


		



	ecs_entity_t roi1 = ecs_new_entity(world, "ROI1");
	ecs_set_pair(world, roi1, Vec2i32, Position, {0, 0});
	ecs_set_pair(world, roi1, Vec2i32, Area, {100, 100});


	ecs_entity_t vid1 = ecs_new_entity(world, "Vid1");
	ecs_set(world, vid1, Device, {"2022-12-15_14-56-29.mp4"});
	ecs_add(world, vid1, VideoReader);
    ecs_add_pair(world, vid1, Action, Open);

	
	ecs_entity_t cap1 = ecs_new_entity(world, "Cap1");
	ecs_add_pair(world, cap1, Capture, vid1);
	ecs_add(world, cap1, Matspec);
	ecs_add(world, cap1, Memory);


	ecs_entity_t snippet1 = ecs_new_entity(world, "Snippet1");
	ecs_add(world, snippet1, Memory);
	ecs_add(world, snippet1, Matspec);
	ecs_add_pair(world, snippet1, Copy, cap1);
	ecs_add_pair(world, snippet1, EcsIsA, roi1);



	ecs_entity_t window1 = ecs_new_entity(world, "Window1");
	ecs_add(world, window1, Window);
	ecs_set_pair(world, window1, Vec2i32, Area, {100, 100});
	ecs_add_pair(world, window1, Draw, snippet1);

	ecs_entity_t window2 = ecs_new_entity(world, "Window2");
	ecs_add(world, window2, Window);
	ecs_set_pair(world, window2, Vec2i32, Area, {100, 100});
	ecs_add_pair(world, window2, Draw, snippet1);

	/*

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
	*/

}