#include "EgDraws.h"
#include "EgTypes.h"
#include "EgMemory.h"
#include "draw.h"
#include <stdio.h>

void System_Draw(ecs_iter_t *it)
{
    Memory *img = ecs_field(it, Memory, 1);
    Matspec *spec = ecs_field(it, Matspec, 2);
    for(int i = 0; i < it->count; ++i)
    {
        char const * name = ecs_get_name(it->world, it->entities[i]);
        //printf("Draw %s %i\n", name, spec[i].size[i]);
		if(img[i].data)
		{	
			draw_show(name, img[i].data, spec + i);
		}
    }
}


void EgDrawsImport(ecs_world_t *world)
{
    ECS_MODULE(world, EgDraws);
    ECS_IMPORT(world, EgTypes);
    ECS_IMPORT(world, EgMemory);

	
    ECS_SYSTEM(world, System_Draw, EcsOnUpdate, Memory, Matspec, eg.types.Window);
}