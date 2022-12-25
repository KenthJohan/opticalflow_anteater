#include "module_weldvisi.h"
#include "oflow.h"
#include "camera.h"
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


void Mve2(ecs_iter_t *it)
{
    Image *d = ecs_field(it, Image, 1);
    Camera *c = ecs_field(it, Camera, 2);
    for(int i = 0; i < it->count; ++i)
    {
        printf("Read from camera %s %s\n", 
        ecs_get_name(it->world, it->entities[i]),
        ecs_get_name(it->world, ecs_field_src(it, 2))
        );
    }
}




void System_Camera_Create(ecs_iter_t *it)
{
    Camera *c = ecs_field(it, Camera, 1);
    for(int i = 0; i < it->count; ++i)
    {
        printf("Create camera %s\n", ecs_get_name(it->world, it->entities[i]));
        camera_create(c+i);
    }
}

void System_Camera_Destroy(ecs_iter_t *it)
{
    Camera *c = ecs_field(it, Camera, 1);
    for(int i = 0; i < it->count; ++i)
    {
        printf("Destroy camera %s\n", ecs_get_name(it->world, it->entities[i]));
        camera_destroy(c+i);
    }
}

void System_Camera_Open(ecs_iter_t *it)
{
    Device *d = ecs_field(it, Device, 1);
    Camera *c = ecs_field(it, Camera, 2);
    for(int i = 0; i < it->count; ++i)
    {
        printf("Open camera: %s\n", d[i].path);
        int r = camera_open(c + i, d[i].path);
        if(r == 0)
        {
            ecs_remove_pair(it->world, it->entities[i], Action, Open);
            ecs_add_pair(it->world, it->entities[i], Status, Open);
            int w = camera_get_int(c + i, CAMERA_PROP_FRAME_WIDTH);
            int h = camera_get_int(c + i, CAMERA_PROP_FRAME_HEIGHT);
            ecs_set_pair(it->world, it->entities[i], Vec2i32, Resolution, {w,h});
        }
        else
        {
            ecs_remove_pair(it->world, it->entities[i], Action, Open);
            ecs_add_pair(it->world, it->entities[i], Status, OpenError);
        }
    }
}

void System_Camera_Close(ecs_iter_t *it)
{
    Device *d = ecs_field(it, Device, 1);
    Camera *c = ecs_field(it, Camera, 2);
    for(int i = 0; i < it->count; ++i)
    {
        printf("Close camera: %s\n", d[i].path);
        int r = camera_close(c + i);
        if(r == 0)
        {
            ecs_remove_pair(it->world, it->entities[i], Action, Close);
            ecs_add_pair(it->world, it->entities[i], Status, Close);
        }
        else
        {
            ecs_remove_pair(it->world, it->entities[i], Action, Close);
            ecs_add_pair(it->world, it->entities[i], Status, CloseError);
        }
    }
}



void System_Camera_Capture(ecs_iter_t *it)
{
    Camera *cam = ecs_field(it, Camera, 1);
    Image *img = ecs_field(it, Image, 2);
    Vec2i32 *res = ecs_field(it, Vec2i32, 3);
    for(int i = 0; i < it->count; ++i)
    {
        int type;
        void * data;
        camera_read(cam + i, &data, res + i, &type);
        img[i].data = data;
        img[i].type = type;
    }
}





ECS_DECLARE(Resolution);
ECS_DECLARE(Position);
ECS_DECLARE(Veclocity);
ECS_DECLARE(CropPosition);
ECS_DECLARE(CropSize);
ECS_DECLARE(Uses);
ECS_DECLARE(Open);
ECS_DECLARE(OpenTry);
ECS_DECLARE(OpenError);
ECS_DECLARE(Close);
ECS_DECLARE(CloseTry);
ECS_DECLARE(CloseError);
ECS_DECLARE(Status);
ECS_DECLARE(Action);
ECS_COMPONENT_DECLARE(Weldvisi_View);
ECS_COMPONENT_DECLARE(Vec2i32);
ECS_COMPONENT_DECLARE(Vec2f32);
ECS_COMPONENT_DECLARE(Device);
ECS_COMPONENT_DECLARE(Image);
ECS_COMPONENT_DECLARE(Camera);

void SimpleModuleImport(ecs_world_t *world)
{
    ECS_MODULE(world, SimpleModule);
    ECS_ENTITY_DEFINE(world, Status, Union);
    ECS_TAG_DEFINE(world, Resolution);
    ECS_TAG_DEFINE(world, Position);
    ECS_TAG_DEFINE(world, Veclocity);
    ECS_TAG_DEFINE(world, CropPosition);
    ECS_TAG_DEFINE(world, CropSize);
    //ECS_TAG_DEFINE(world, Uses);
    ECS_ENTITY_DEFINE(world, Uses, EcsAcyclic);
    ECS_TAG_DEFINE(world, Open);
    ECS_TAG_DEFINE(world, OpenTry);
    ECS_TAG_DEFINE(world, OpenError);
    ECS_TAG_DEFINE(world, Close);
    ECS_TAG_DEFINE(world, CloseTry);
    ECS_TAG_DEFINE(world, CloseError);
    ECS_TAG_DEFINE(world, Action);
    ECS_COMPONENT_DEFINE(world, Weldvisi_View);
    ECS_COMPONENT_DEFINE(world, Vec2i32);
    ECS_COMPONENT_DEFINE(world, Vec2f32);
    ECS_COMPONENT_DEFINE(world, Device);
    ECS_COMPONENT_DEFINE(world, Image);
    ECS_COMPONENT_DEFINE(world, Camera);

    //ECS_SYSTEM(world, Move, EcsOnUpdate, Weldvisi_View, (Vec2i32, CropPosition), (Vec2i32, CropSize));
    ECS_SYSTEM(world, System_Camera_Capture, EcsOnUpdate, Camera, Image, (Vec2i32, Resolution));
    //ECS_SYSTEM(world, System_Capture, EcsOnUpdate, Image, Image(cascade(Uses)));
    ECS_OBSERVER(world, System_Camera_Create, EcsOnAdd, Camera);
    ECS_OBSERVER(world, System_Camera_Destroy, EcsOnRemove, Camera);
    ECS_SYSTEM(world, System_Camera_Open, EcsOnUpdate, Device, Camera, (Action, Open));
    ECS_SYSTEM(world, System_Camera_Close, EcsOnUpdate, Camera, (Action, Close));

    ecs_struct(world, {
        .entity = ecs_id(Vec2i32),
        .members = {
            { .name = "x", .type = ecs_id(ecs_i32_t) },
            { .name = "y", .type = ecs_id(ecs_i32_t) }
        }
    });
    ecs_struct(world, {
        .entity = ecs_id(Vec2f32),
        .members = {
            { .name = "x", .type = ecs_id(ecs_f32_t) },
            { .name = "y", .type = ecs_id(ecs_f32_t) }
        }
    });
    ecs_struct(world, {
        .entity = ecs_id(Device),
        .members = {
            { .name = "path", .type = ecs_id(ecs_string_t) }
        }
    });
}