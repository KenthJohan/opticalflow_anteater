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
    ecs_entity_t   f1_src  = ecs_field_src(it, 1); // Shared
    Mat          * f1_mat  = ecs_field(it, Mat, 1); // Shared
    Mat          * f2_mat  = ecs_field(it, Mat, 2);
    int            f2_self = ecs_field_is_self(it, 2);
    Vec2i32      * f3_pos  = ecs_field(it, Vec2i32, 3);
    if(f1_mat->start == NULL){return;}
    for(int i = 0; i < it->count; ++i)
    {
        Mat *matsrc = f2_mat + f2_self*i;
        char const * f1_name = ecs_get_name(it->world, f1_src);
        char const * fi_name = ecs_get_name(it->world, it->entities[i]);
        printf("draw_weighed: %s, %s, %i, (%i %i) %i\n", f1_name, fi_name, it->count, f3_pos[i].x, f3_pos[i].y, matsrc->dims);
        if(matsrc->start == NULL) {continue;}
        // Region of interest:
        // https://github.com/opencv/opencv/blob/9390c56831270a94af6480feb4cc330e4aa2ee5e/modules/core/src/matrix.cpp#L798
        // https://github.com/opencv/opencv/blob/d9a444ca1a97740f9d092816a0ad0a523482911f/modules/core/include/opencv2/core/mat.hpp#L2622
        Mat dst = *f1_mat;
        int32_t x = EG_CLAMP(f3_pos[i].x, 0, f1_mat->shape[1] - matsrc->shape[1]);
        int32_t y = EG_CLAMP(f3_pos[i].y, 0, f1_mat->shape[0] - matsrc->shape[0]);
        dst.start += (y * f1_mat->step[0]) + (x * f1_mat->step[1]);
        dst.shape[0] = matsrc->shape[0];
        dst.shape[1] = matsrc->shape[1];
        draw_weighed(matsrc, 1.0, &dst, 0.0, 0.0, &dst);

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