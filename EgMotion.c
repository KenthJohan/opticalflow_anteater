#include "EgMotion.h"
#include "oflow.h"
#include "VideoReader.h"
#include "EgMemory.h"
#include "EgTypes.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>



ECS_COMPONENT_DECLARE(MotionEstimator);


void System_Test(ecs_iter_t *it)
{
    Mat *d = ecs_field(it, Mat, 1);
    VideoReader *c = ecs_field(it, VideoReader, 2);
    for(int i = 0; i < it->count; ++i)
    {
        printf("Read from camera %s %s\n", 
        ecs_get_name(it->world, it->entities[i]),
        ecs_get_name(it->world, ecs_field_src(it, 2))
        );
    }
}


void System_Motion_Estimation(ecs_iter_t *it)
{
    //printf("System_Motion_Estimation %i\n", it->count);
    MotionEstimator *v = ecs_field(it, MotionEstimator, 1);
    Vec2i32 *rio_pos = ecs_field(it, Vec2i32, 2);
    Vec2i32 *rio_len = ecs_field(it, Vec2i32, 3);
    Vec2i32 *vel = ecs_field(it, Vec2i32, 4);
    Mat *img = ecs_field(it, Mat, 5); // Shared
    Vec2i32 *res = ecs_field(it, Vec2i32, 6);  // Shared
    for(int i = 0; i < it->count; ++i)
    {
        //printf("%s:\n", ecs_get_name(it->world, it->entities[i]));
        //printf("%s: %i %i\n", ecs_get_name(it->world, it->entities[i]), res[i].x, res[i].y);
        float alpha = 0.1f;
        //oflow_run(&v[i].context, img[0].data, img[0].type, res[0], vel + i, alpha, rio_pos[i], rio_len[i]);
    }
}


void System_Oflow_Create(ecs_iter_t *it)
{
    MotionEstimator *v = ecs_field(it, MotionEstimator, 1);
    Mat *img = ecs_field(it, Mat, 2); // Shared
    Vec2i32 *res = ecs_field(it, Vec2i32, 3);  // Shared
    for(int i = 0; i < it->count; ++i)
    {
        printf("oflow_init %s\n", ecs_get_name(it->world, it->entities[i]));
        //oflow_init(&v[i].context, img[0].data, img[0].type, res[0]);
    }
}

void System_Oflow_Destroy(ecs_iter_t *it)
{
    MotionEstimator *v = ecs_field(it, MotionEstimator, 1);
    for(int i = 0; i < it->count; ++i)
    {
        printf("Destroy oflow %s\n", ecs_get_name(it->world, it->entities[i]));
    }
}


void mul(Vec2f32 * a, Vec2f32 * b, Vec2f32 * r)
{
    Vec2f32 q = {a->x * b->x - a->y * b->y, a->x * b->y + a->y * b->x};
    *r = q;
}


void System_Spinner(ecs_iter_t *it)
{
    Vec2f32 *v = ecs_field(it, Vec2f32, 1);
    float a = (2*M_PI) / 360.0;
    Vec2f32 const r = {cos(a), sin(a)};
    for(int i = 0; i < it->count; ++i)
    {
        mul(v+i, &r, v+i);
    }
}


void EgMotionImport(ecs_world_t *world)
{
    ECS_MODULE(world, EgMotion);
    ECS_IMPORT(world, EgTypes);
    ECS_IMPORT(world, EgMemory);


    ECS_COMPONENT_DEFINE(world, MotionEstimator);

    ECS_SYSTEM(world, System_Spinner, EcsOnUpdate, (Vec2f32, eg.types.Velocity));

    /*
    ecs_system(world, {
        .entity = ecs_entity(world, {
            .name = "System_Motion_Estimation",
            .add = { ecs_dependson(EcsOnUpdate) }
        }),
        .query.filter.instanced = true,
        .query.filter.terms = {
            {.id = ecs_id(Weldvisi_View), .inout = EcsIn },
            {.id = ecs_pair(ecs_id(Vec2i32), CropPosition), .inout = EcsIn },
            {.id = ecs_pair(ecs_id(Vec2i32), CropSize), .inout = EcsIn },
            {.id = ecs_pair(ecs_id(Vec2f32), Velocity), .inout = EcsInOut },
            {.id = ecs_id(Mat), .inout = EcsIn, .src.flags = EcsUp, .src.trav = Uses},
            {.id = ecs_pair(ecs_id(Vec2i32), Resolution), .inout = EcsIn,.src.flags = EcsUp,.src.trav = Uses}
        },
        .callback = System_Motion_Estimation
    });
    */
    
    /*
    ECS_SYSTEM(world, System_Motion_Estimation, EcsOnUpdate, 
    Weldvisi_View, 
    (Vec2i32, CropPosition), 
    (Vec2i32, CropSize), 
    (Vec2f32, Velocity), 
    Image(up(Uses)),
    Vec2i32(up(Uses), Resolution)
    );
    */
   

    //ECS_OBSERVER(world, System_Oflow_Create, EcsOnAdd, Weldvisi_View, Mat(up(eg.types.Uses)), Vec2i32(up(eg.types.Uses), eg.types.Resolution));
    //ECS_OBSERVER(world, System_Oflow_Destroy, EcsOnRemove, Weldvisi_View);


    





}