#include "mainer.h"
#include "EgMotion.h"
#include "EgMemory.h"
#include "EgVideo.h"
#include "EgTypes.h"
#include "EgDraws.h"
#include <stdio.h>

void my_abort(void)
{
	printf("Aborted\n");
    exit(EXIT_FAILURE);
}


void mainer(int argc, char* argv[])
{
	ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.abort_ = my_abort;
    ecs_os_set_api(&os_api);

    ecs_world_t *world = ecs_init_w_args(argc, argv);

	ecs_log_set_level(1);
    ECS_IMPORT(world, EgMotion);
    ECS_IMPORT(world, EgMemory);
    ECS_IMPORT(world, EgVideo);
    ECS_IMPORT(world, EgTypes);
    ECS_IMPORT(world, EgDraws);

	//https://www.flecs.dev/explorer/?remote=true
    ecs_singleton_set(world, EcsRest, {0});

	ecs_entity_t roi1 = ecs_new_entity(world, "roi1");
	ecs_set_pair(world, roi1, Vec2i32, Position, {640*0, 0});
	ecs_set_pair(world, roi1, Vec2i32, Area, {640, 400});
	ecs_entity_t roi2 = ecs_new_entity(world, "roi2");
	ecs_set_pair(world, roi2, Vec2i32, Position, {640*1, 0});
	ecs_set_pair(world, roi2, Vec2i32, Area, {640, 400});
	ecs_entity_t roi3 = ecs_new_entity(world, "roi3");
	ecs_set_pair(world, roi3, Vec2i32, Position, {640*2, 0});
	ecs_set_pair(world, roi3, Vec2i32, Area, {640, 400});


	ecs_entity_t vid1 = ecs_new_entity(world, "Vid1");
	ecs_set(world, vid1, Device, {"2022-12-15_14-56-29.mp4"});
	ecs_add(world, vid1, VideoReader);
    ecs_add_pair(world, vid1, Action, Open);
	
	ecs_entity_t cap1 = ecs_new_entity(world, "Cap1");
	ecs_add_pair(world, cap1, Capture, vid1);
	ecs_add(world, cap1, Mat);

	ecs_entity_t snippet1 = ecs_new_entity(world, "Snippet1");
	ecs_add(world, snippet1, Mat);
	ecs_entity_t snippet2 = ecs_new_entity(world, "Snippet2");
	ecs_add(world, snippet2, Mat);
	ecs_entity_t snippet3 = ecs_new_entity(world, "Snippet3");
	ecs_add(world, snippet3, Mat);

	ecs_entity_t copy1 = ecs_new_entity(world, "Copy1");
	ecs_add_pair(world, copy1, CopyFrom, cap1);
	ecs_add_pair(world, copy1, CopyTo, snippet1);
	ecs_add_pair(world, copy1, EcsIsA, roi1);

	ecs_entity_t copy2 = ecs_new_entity(world, "Copy2");
	ecs_add_pair(world, copy2, CopyFrom, cap1);
	ecs_add_pair(world, copy2, CopyTo, snippet2);
	ecs_add_pair(world, copy2, EcsIsA, roi2);

	ecs_entity_t copy3 = ecs_new_entity(world, "Copy3");
	ecs_add_pair(world, copy3, CopyFrom, cap1);
	ecs_add_pair(world, copy3, CopyTo, snippet3);
	ecs_add_pair(world, copy3, EcsIsA, roi3);

	ecs_entity_t window1 = ecs_new_entity(world, "Window1");
	ecs_add(world, window1, Window);
	//ecs_set(world, window1, Mat, {.shape = {400,1920,0,0}});
	ecs_set(world, window1, Mat, {.shape = {400,1920,0,0}});
	//ecs_set_pair(world, window2, Vec2i32, Resolution, {100, 100});

	/*
	ecs_entity_t drawsnippet1 = ecs_new_entity(world, "DrawSnippet1");
	ecs_set_pair(world, drawsnippet1, Vec2i32, Position, {640*0 + 10, 0});
	ecs_add_pair(world, drawsnippet1, EcsIsA, snippet1);
	ecs_add_pair(world, drawsnippet1, EcsChildOf, window1);

	ecs_entity_t drawsnippet2 = ecs_new_entity(world, "drawsnippet2");
	ecs_set_pair(world, drawsnippet2, Vec2i32, Position, {640*1 + 20, 0});
	ecs_add_pair(world, drawsnippet2, EcsIsA, snippet2);
	ecs_add_pair(world, drawsnippet2, EcsChildOf, window1);

	ecs_entity_t drawsnippet3 = ecs_new_entity(world, "drawsnippet3");
	ecs_set_pair(world, drawsnippet3, Vec2i32, Position, {640*2 + 30, 0});
	ecs_add_pair(world, drawsnippet3, EcsIsA, snippet3);
	ecs_add_pair(world, drawsnippet3, EcsChildOf, window1);
	*/

	
	ecs_entity_t drawvid = ecs_new_entity(world, "drawvid");
	ecs_set_pair(world, drawvid, Vec2i32, Position, {0, 0});
	ecs_add_pair(world, drawvid, EcsIsA, cap1);
	ecs_add_pair(world, drawvid, EcsChildOf, window1);
	
	ecs_entity_t drawrect1 = ecs_new_entity(world, "drawrect1");
	ecs_add_pair(world, drawrect1, EcsIsA, roi1);
	ecs_add_pair(world, drawrect1, EcsChildOf, window1);

	ecs_entity_t drawrect2 = ecs_new_entity(world, "drawrect2");
	ecs_add_pair(world, drawrect2, EcsIsA, roi2);
	ecs_add_pair(world, drawrect2, EcsChildOf, window1);

	ecs_entity_t drawrect3 = ecs_new_entity(world, "drawrect3");
	ecs_add_pair(world, drawrect3, EcsIsA, roi3);
	ecs_add_pair(world, drawrect3, EcsChildOf, window1);

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


    while(true)
    {
        {
            //int keyboard = cv::waitKey(30);
            //if (keyboard == 'q' || keyboard == 27) {break;}
        }
        ecs_progress(world, 0);
        //ecs_sleepf(0.1f);
    }

	ecs_log_set_level(0);
    ecs_fini(world);
}