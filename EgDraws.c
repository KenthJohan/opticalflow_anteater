#include "EgDraws.h"
#include "EgTypes.h"
#include "EgMemory.h"
#include "draw.h"
#include <stdio.h>

void System_Show(ecs_iter_t *it)
{
    Mat *mat = ecs_field(it, Mat, 1);
    for(int i = 0; i < it->count; ++i)
    {
        if(mat[i].data == NULL){continue;}
        char const * name = ecs_get_name(it->world, it->entities[i]);
		draw_show(name, mat + i);
    }
}

void System_Add(ecs_iter_t *it)
{
    ecs_entity_t e0 = ecs_field_src(it, 1); // Shared
    Mat *mat0 = ecs_field(it, Mat, 1); // Shared
    Mat *mat = ecs_field(it, Mat, 2);
    if(mat0->data == NULL){return;}
    for(int i = 0; i < it->count; ++i)
    {
        char const * name0 = ecs_get_name(it->world, ecs_field_src(it, 1));
        char const * name = ecs_get_name(it->world, it->entities[i]);
        //printf("draw_weighed: %s, %s\n", name0, name);

        /*
        if(mem[i].size != reqsize)
        {
            printf("%s: Reqsize %i\n", name, reqsize);
            //ecs_os_free(mem[i].data);
            mem[i].data = ecs_os_malloc(reqsize);
            mem[i].size = reqsize;
        }
        printf("%s: Copyfrom %s\n", name, name0);
        spec[i].type = spec0[0].type;
        spec[i].dims = spec0[0].dims;
        spec[i].size[0] = area[i].y;
        spec[i].size[1] = area[i].x;
        spec[i].step[0] = area[i].x * spec0[0].step[1];
        spec[i].step[1] = spec0[0].step[1];

        draw_weighed(img0, spec0, 0.5, img[i].data, spec + i, 0.5, 0.0, img[i].data, spec + i);
        */
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
            .name = "System_Show",
            .add = { ecs_dependson(EcsOnUpdate) }
        }),
        .query.filter.instanced = true,
        .query.filter.terms = {
            {.id = ecs_id(Mat)},
            {.id = ecs_id(Window)}
        },
        .callback = System_Show
    });
    
    ecs_system(world, {
        .entity = ecs_entity(world, {
            .name = "System_Add",
            .add = { ecs_dependson(EcsOnUpdate) }
        }),
        .query.filter.instanced = true,
        .query.filter.terms = {
            {.id = ecs_id(Mat), .inout = EcsInOut, .src.trav = Draw, .src.flags = EcsUp },
            {.id = ecs_id(Mat), .inout = EcsInOut },
        },
        .callback = System_Add
    });
    
}