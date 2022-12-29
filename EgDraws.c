#include "EgDraws.h"
#include "EgTypes.h"
#include "EgMemory.h"
#include "draw.h"
#include <stdio.h>

void System_Draw(ecs_iter_t *it)
{
    Memory *img = ecs_field(it, Memory, 1);
    Vec2i32 *res = ecs_field(it, Vec2i32, 2);
    for(int i = 0; i < it->count; ++i)
    {
        char const * name = ecs_get_name(it->world, it->entities[i]);
        printf("Draw %s %p\n", name, img[i].data);
		if(img[i].data)
		{	
			draw_show(name, img[i].data, img[i].type, res[i]);
		}
    }
}


void EgDrawsImport(ecs_world_t *world)
{
    ECS_MODULE(world, EgDraws);
    ECS_IMPORT(world, EgTypes);
    ECS_IMPORT(world, EgMemory);

	
    ECS_SYSTEM(world, System_Draw, EcsOnUpdate, Memory, (eg.types.Vec2i32, eg.types.Resolution), eg.types.Window);
}