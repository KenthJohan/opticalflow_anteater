#include "EgDraws.h"
#include "EgTypes.h"
#include "EgMemory.h"
#include "draw.h"
#include <stdio.h>

void System_Draw(ecs_iter_t *it)
{
    ecs_entity_t e0 = ecs_field_src(it, 1); // Shared
    Memory *img0 = ecs_field(it, Memory, 1); // Shared
    Matspec *spec0 = ecs_field(it, Matspec, 2); // Shared
    if(img0->data == NULL){return;}
    for(int i = 0; i < it->count; ++i)
    {
        char const * name0 = ecs_get_name(it->world, e0);
        char const * name = ecs_get_name(it->world, it->entities[i]);
        printf("Draw %s %s %i\n", name0, name, spec0->size[i]);
		draw_show(name, img0->data, spec0);
    }
}


void EgDrawsImport(ecs_world_t *world)
{
    ECS_MODULE(world, EgDraws);
    ECS_IMPORT(world, EgTypes);
    ECS_IMPORT(world, EgMemory);

	
    //ECS_SYSTEM(world, System_Draw, EcsOnUpdate, Memory, Matspec, eg.types.Window);


    ecs_system(world, {
        .entity = ecs_entity(world, {
            .name = "System_Draw",
            .add = { ecs_dependson(EcsOnUpdate) }
        }),
        .query.filter.instanced = true,
        .query.filter.terms = {
            {.id = ecs_id(Memory), .inout = EcsInOut, .src.trav = Draw, .src.flags = EcsUp },
            {.id = ecs_id(Matspec), .inout = EcsInOut, .src.trav = Draw, .src.flags = EcsUp },
            {.id = ecs_id(Window)}
        },
        .callback = System_Draw
    });
    

}