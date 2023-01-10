#include "EgDraws.h"
#include "EgTypes.h"
#include "EgMemory.h"
#include "draw.h"
#include "common.h"
#include <stdio.h>



void System_Show(ecs_iter_t *it)
{
    Mat *mat = ecs_field(it, Mat, 1);
    for(int i = 0; i < it->count; ++i)
    {
        if(mat[i].start == NULL){continue;}
        char const * name = ecs_get_name(it->world, it->entities[i]);
		draw_show(name, mat + i);
    }
}

void System_Add(ecs_iter_t *it)
{
    ecs_entity_t e0 = ecs_field_src(it, 1); // Shared
    Mat *mat0 = ecs_field(it, Mat, 1); // Shared
    Mat *mat = ecs_field(it, Mat, 2);
    Vec2i32 *pos = ecs_field(it, Vec2i32, 3);
    if(mat0->start == NULL){return;}
    for(int i = 0; i < it->count; ++i)
    {
        char const * name0 = ecs_get_name(it->world, ecs_field_src(it, 1));
        char const * name = ecs_get_name(it->world, it->entities[i]);
        //printf("draw_weighed: %s, %s\n", name0, name);
        if(mat[i].start == NULL) {continue;}
        Mat dst = *mat0;
        int32_t x = EG_CLAMP(pos[i].x, 0, mat0->shape[1] - mat[i].shape[1]);
        int32_t y = EG_CLAMP(pos[i].y, 0, mat0->shape[0] - mat[i].shape[0]);
        dst.start += (y * mat0->step[0]) + (x * mat0->step[1]);
        dst.shape[0] = mat[i].shape[0];
        dst.shape[1] = mat[i].shape[1];
        draw_weighed(mat + i, 1.0, &dst, 0.0, 0.0, &dst);
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
            {.id = ecs_id(Mat), .inout = EcsInOut, .src.trav = EcsChildOf, .src.flags = EcsUp },
            {.id = ecs_id(Mat), .inout = EcsIn },
            {.id = ecs_pair(ecs_id(Vec2i32), Position), .inout = EcsIn }
        },
        .callback = System_Add
    });
    
}