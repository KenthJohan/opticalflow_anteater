#include "module_weldvisi.h"
#include "oflow.h"
#include <stdio.h>

void Move(ecs_iter_t *it)
{
    Weldvisi_View *v = ecs_field(it, Weldvisi_View, 1);
    Vec2i32 *p = ecs_field(it, Vec2i32, 2);
    Vec2i32 *r = ecs_field(it, Vec2i32, 3);
    for(int i = 0; i < it->count; ++i)
    {
        printf("Move: %i %i\n", p[i].x, p[i].y);
        //void * context;
        //void * raw;
        //oflow_run(context, raw, 0, r[i], v + i, 0.1f, p[i], r[i]);
    }
}


void System_Capture(ecs_iter_t *it)
{

}

void System_Open_Camera(ecs_iter_t *it)
{

}


ECS_DECLARE(Resolution);
ECS_DECLARE(Position);
ECS_DECLARE(Veclocity);
ECS_DECLARE(CropPosition);
ECS_DECLARE(CropSize);
ECS_DECLARE(Uses);
ECS_DECLARE(Open);
ECS_DECLARE(Close);
ECS_DECLARE(Try);
ECS_COMPONENT_DECLARE(Weldvisi_View);
ECS_COMPONENT_DECLARE(Vec2i32);
ECS_COMPONENT_DECLARE(Vec2f32);
ECS_COMPONENT_DECLARE(Device);
ECS_COMPONENT_DECLARE(Image);
ECS_COMPONENT_DECLARE(Camera);

void SimpleModuleImport(ecs_world_t *world)
{
    ECS_MODULE(world, SimpleModule);
    ECS_TAG_DEFINE(world, Resolution);
    ECS_TAG_DEFINE(world, Position);
    ECS_TAG_DEFINE(world, Veclocity);
    ECS_TAG_DEFINE(world, CropPosition);
    ECS_TAG_DEFINE(world, CropSize);
    ECS_TAG_DEFINE(world, Uses);
    ECS_TAG_DEFINE(world, Open);
    ECS_TAG_DEFINE(world, Close);
    ECS_TAG_DEFINE(world, Try);
    ECS_COMPONENT_DEFINE(world, Weldvisi_View);
    ECS_COMPONENT_DEFINE(world, Vec2i32);
    ECS_COMPONENT_DEFINE(world, Vec2f32);
    ECS_COMPONENT_DEFINE(world, Device);
    ECS_COMPONENT_DEFINE(world, Image);
    ECS_COMPONENT_DEFINE(world, Camera);

    ECS_SYSTEM(world, Move, EcsOnUpdate, Weldvisi_View, (Vec2i32, CropPosition), (Vec2i32, CropSize));
    ECS_SYSTEM(world, System_Capture, EcsOnUpdate, Camera, (Vec2i32, CropSize));
    ECS_SYSTEM(world, System_Open_Camera, EcsOnUpdate, Camera, (Open, Try));
}