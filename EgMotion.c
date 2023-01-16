#include "EgMotion.h"
#include "deps/opencv/motionest_phasecorr.h"
#include "draw.h"
#include "VideoReader.h"
#include "EgMats.h"
#include "EgTypes.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>



ECS_COMPONENT_DECLARE(MotionEstimator);




void System_Motionest_Estimate(ecs_iter_t *it)
{
    //printf("System_Motionest_Estimate %i\n", it->count);
    MotionEstimator *mot_field = ecs_field(it, MotionEstimator, 1);
    Tensor2_U8C3             *img_field = ecs_field(it, Tensor2_U8C3, 2);
    int              img_self = ecs_field_is_self(it, 2);
    Vec2f32         *vel_field = ecs_field(it, Vec2f32, 3);
    int              vel_self = ecs_field_is_self(it, 3);
    for(int i = 0; i < it->count; ++i)
    {
        Tensor2_U8C3             *img = img_field + i * img_self;
        MotionEstimator *mot = mot_field + i;
        Vec2f32         *vel = vel_field + i * vel_self;

        //printf("Tensor2_U8C3: %i\n", img->size);
        if(img->start == NULL) {continue;}

        float alpha = 0.1f;
        motionest_phasecorr_run(&mot->context, img, vel, alpha);
    }
}


void Observer_Motionest_Create(ecs_iter_t *it)
{
    MotionEstimator *mot_field = ecs_field(it, MotionEstimator, 1);
    Tensor2_U8C3             *img_field = ecs_field(it, Tensor2_U8C3, 2); 
    int              img_self = ecs_field_is_self(it, 2);
    for(int i = 0; i < it->count; ++i)
    {
        Tensor2_U8C3             *img = img_field + i * img_self;
        MotionEstimator *mot = mot_field + i;
        //printf("Tensor2_U8C3: %i\n", img->size);
        //if(img->start == NULL) {continue;}
        printf("Observer_Motionest_Create %s\n", ecs_get_name(it->world, it->entities[i]));
        motionest_phasecorr_init(&mot->context, img);
    }
}

void Observer_Motionest_Destroy(ecs_iter_t *it)
{
    MotionEstimator *v = ecs_field(it, MotionEstimator, 1);
    for(int i = 0; i < it->count; ++i)
    {
        printf("Observer_Motionest_Destroy %s\n", ecs_get_name(it->world, it->entities[i]));
    }
}


void mul(Vec2f32 const * a, Vec2f32 const* b, Vec2f32 * r)
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
    ECS_IMPORT(world, EgMats);


    ECS_COMPONENT_DEFINE(world, MotionEstimator);


    ecs_struct(world, {
        .entity = ecs_id(MotionEstimator),
        .members = {
            { .name = "dummy", .type = ecs_id(ecs_i32_t) },
            { .name = "context", .type = ecs_id(ecs_uptr_t) }
        }
    });












    //ECS_SYSTEM(world, System_Spinner, EcsOnUpdate, (Vec2f32, eg.types.Velocity));
    ECS_SYSTEM(world, System_Motionest_Estimate, EcsOnUpdate, MotionEstimator, Tensor2_U8C3, (Vec2f32, eg.types.Velocity));
    ECS_OBSERVER(world, Observer_Motionest_Create, EcsOnAdd, MotionEstimator, Tensor2_U8C3);
    ECS_OBSERVER(world, Observer_Motionest_Destroy, EcsOnRemove, MotionEstimator);

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
            {.id = ecs_id(Tensor2_U8C3), .inout = EcsIn, .src.flags = EcsUp, .src.trav = Uses},
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
   




    





}