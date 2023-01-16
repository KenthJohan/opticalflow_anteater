#include "EgMotion.h"
#include "EgTensors.h"
#include "EgVideo.h"
#include "EgTypes.h"
#include "EgDraws.h"
#include <stdio.h>

void my_abort(void)
{
	printf("Aborted\n");
    exit(EXIT_FAILURE);
}


void main(int argc, char* argv[])
{
	ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.abort_ = my_abort;
    ecs_os_set_api(&os_api);

    ecs_world_t *world = ecs_init_w_args(argc, argv);
    
	ecs_log_set_level(1);
    ECS_IMPORT(world, EgMotion);
    ECS_IMPORT(world, EgTensors);
    ECS_IMPORT(world, EgVideo);
    ECS_IMPORT(world, EgTypes);
    ECS_IMPORT(world, EgDraws);

	//https://www.flecs.dev/explorer/?remote=true
    ecs_singleton_set(world, EcsRest, {0});
	ecs_plecs_from_file(world, "config.flecs");
	
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