#include "EgDraws.h"
#include "EgTypes.h"
#include "EgMemory.h"
#include "draw.h"
#include "common.h"
#include "mat.h"
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
    ecs_entity_t   dst_ent        = ecs_field_src(it, 1); // Shared
    Mat          * dst_mat        = ecs_field(it, Mat, 1); // Shared
    Mat          * src_mat_field  = ecs_field(it, Mat, 2); // Shared?
    int            src_mat_self   = ecs_field_is_self(it, 2); // Shared?
    Vec2i32      * pos            = ecs_field(it, Vec2i32, 3);
    if(dst_mat->start == NULL){return;}
    for(int i = 0; i < it->count; ++i)
    {
        Mat *src_mat = src_mat_field + src_mat_self*i;
        char const * f1_name = ecs_get_name(it->world, dst_ent);
        char const * fi_name = ecs_get_name(it->world, it->entities[i]);
        printf("draw_weighed: %s, %s, %i, (%i %i) %i\n", f1_name, fi_name, it->count, pos[i].x, pos[i].y, src_mat->dims);
        if(src_mat->start == NULL) {continue;}
        Mat dst_mat_rio; // Matrix region of interest
        mat_roi(src_mat, pos + i, dst_mat, &dst_mat_rio);
        draw_weighed(src_mat, 1.0, &dst_mat_rio, 0.0, 0.0, &dst_mat_rio);
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