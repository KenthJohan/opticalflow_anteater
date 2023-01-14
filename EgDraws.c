#include "EgDraws.h"
#include "EgTypes.h"
#include "EgMats.h"
#include "draw.h"
#include "common.h"
#include "mat.h"
#include <stdio.h>



void System_Window_Show(ecs_iter_t *it)
{
    Mat *mat = ecs_field(it, Mat, 1);
    for(int i = 0; i < it->count; ++i)
    {
        if(mat[i].start == NULL){continue;}
        char const * name = ecs_get_name(it->world, it->entities[i]);
		draw_show(name, mat + i);
    }
}



void System_Draw_Image(ecs_iter_t *it)
{
    //ecs_entity_t   dst_ent        = ecs_field_src(it, 1); // Shared
    Mat          * dst_mat        = ecs_field(it, Mat, 1); // Shared
    Mat          * src_mat_field  = ecs_field(it, Mat, 2); // Shared?
    int            src_mat_self   = ecs_field_is_self(it, 2); // Shared?
    Vec2i32      * pos            = ecs_field(it, Vec2i32, 3);
    if(dst_mat->start == NULL){return;}
    for(int i = 0; i < it->count; ++i)
    {
        Mat *src_mat = src_mat_field + src_mat_self*i;
        //char const * f1_name = ecs_get_name(it->world, dst_ent);
        //char const * fi_name = ecs_get_name(it->world, it->entities[i]);
        //printf("draw_weighed: %s, %s, %i, (%i %i) %i\n", f1_name, fi_name, it->count, pos[i].x, pos[i].y, src_mat->dims);
        if(src_mat->start == NULL) {continue;}
        Mat dst_mat_rio = *dst_mat; // Matrix region of interest
        Mat src_mat_rio = *src_mat; // Matrix region of interest
        mat_roi_offset(&dst_mat_rio, pos + i);
        mat_roi_fit(&dst_mat_rio, &src_mat_rio);
        //mat_roi(&src_mat_rio, pos + i, dst_mat, &dst_mat_rio);
        draw_weighed(&src_mat_rio, 1.0, &dst_mat_rio, 0.0, 0.0, &dst_mat_rio);
    }
}


void System_Draw_Rectangle(ecs_iter_t *it)
{
    Mat           * dst = ecs_field(it, Mat, 1); // Shared
    Vec2i32 const * pos = ecs_field(it, Vec2i32, 2); // Shared?
    Vec2i32 const * rec = ecs_field(it, Vec2i32, 3); // Shared?
    int             pos_self = ecs_field_is_self(it, 2); // Shared?
    int             rec_self = ecs_field_is_self(it, 3); // Shared?
    if(dst->start == NULL){return;}
    for(int i = 0; i < it->count; ++i)
    {
        Vec2i32 const * p = pos + i * pos_self;
        Vec2i32 const * r = rec + i * rec_self;
        draw_rectangle(dst, p, r);
    }
}

void System_Draw_Direction(ecs_iter_t *it)
{
    Mat           * dst = ecs_field(it, Mat, 1); // Shared
    Vec2i32 const * pos = ecs_field(it, Vec2i32, 2); // Shared?
    Vec2f32 const * dir = ecs_field(it, Vec2f32, 3); // Shared?
    int             pos_self = ecs_field_is_self(it, 2); // Shared?
    int             dir_self = ecs_field_is_self(it, 3); // Shared?
    if(dst->start == NULL){return;}
    for(int i = 0; i < it->count; ++i)
    {
        Vec2i32 const * p = pos + i * pos_self;
        Vec2f32 const * d = dir + i * dir_self;
        draw_arrow(dst, p, &(Vec2i32){d->x, d->y}, 1.0);
    }
}


void EgDrawsImport(ecs_world_t *world)
{
    ECS_MODULE(world, EgDraws);
    ECS_IMPORT(world, EgTypes);
    ECS_IMPORT(world, EgMats);

	
    //ECS_SYSTEM(world, System_Draw, EcsOnUpdate, Memory, Matspec, eg.types.Window);


    ecs_system(world, {
        .entity = ecs_entity(world, {
            .name = "System_Window_Show",
            .add = { ecs_dependson(EcsOnUpdate) }
        }),
        .query.filter.instanced = true,
        .query.filter.terms = {
            {.id = ecs_id(Mat)},
            {.id = ecs_id(Window)}
        },
        .callback = System_Window_Show
    });
    
    ecs_system(world, {
        .entity = ecs_entity(world, {
            .name = "System_Draw_Image",
            .add = { ecs_dependson(EcsOnUpdate) }
        }),
        .query.filter.instanced = true,
        .query.filter.terms = {
            {.id = ecs_id(Mat), .inout = EcsInOut, .src.trav = EcsChildOf, .src.flags = EcsUp },
            {.id = ecs_id(Mat), .inout = EcsIn },
            {.id = ecs_pair(ecs_id(Vec2i32), Position), .inout = EcsIn }
        },
        .callback = System_Draw_Image
    });


    ecs_system(world, {
        .entity = ecs_entity(world, {
            .name = "System_Draw_Rectangle",
            .add = { ecs_dependson(EcsOnUpdate) }
        }),
        .query.filter.instanced = true,
        .query.filter.terms = {
            {.id = ecs_id(Mat), .inout = EcsInOut, .src.trav = EcsChildOf, .src.flags = EcsUp },
            {.id = ecs_pair(ecs_id(Vec2i32), Position), .inout = EcsIn },
            {.id = ecs_pair(ecs_id(Vec2i32), Area), .inout = EcsIn }
        },
        .callback = System_Draw_Rectangle
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .name = "System_Draw_Direction",
            .add = { ecs_dependson(EcsOnUpdate) }
        }),
        .query.filter.instanced = true,
        .query.filter.terms = {
            {.id = ecs_id(Mat), .inout = EcsInOut, .src.trav = EcsChildOf, .src.flags = EcsUp },
            {.id = ecs_pair(ecs_id(Vec2i32), Position), .inout = EcsIn },
            {.id = ecs_pair(ecs_id(Vec2f32), Velocity), .inout = EcsIn }
        },
        .callback = System_Draw_Direction
    });
}