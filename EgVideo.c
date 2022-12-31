#include "EgVideo.h"
#include "EgTypes.h"
#include "EgMemory.h"
#include "camera.h"
#include <stdio.h>

ECS_COMPONENT_DECLARE(Device);
ECS_COMPONENT_DECLARE(Camera);








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
        Vec2i32 res;
        char const * name = ecs_get_name(it->world, it->entities[i]);
        printf("Open camera %s: device: %s\n", name, d[i].path);
        int r = camera_open(c + i, d[i].path);
        if(r == 0)
        {
            ecs_remove_pair(it->world, it->entities[i], Action, Open);
            ecs_add_pair(it->world, it->entities[i], Status, Open);
            
            /*
            res.x = camera_get_int(c + i, CAMERA_PROP_FRAME_WIDTH);
            res.y = camera_get_int(c + i, CAMERA_PROP_FRAME_HEIGHT);
            int t = camera_get_int(c + i, CAMERA_CAP_PROP_FORMAT);
            */
           
            Memory mem;
            camera_read(c + i, &mem, &res);
            ecs_set_pair(it->world, it->entities[i], Vec2i32, Resolution, {res.x, res.y});
            ecs_set_ptr(it->world, it->entities[i], Memory, &mem);

            /*
            printf("Camera %s: %ix%ix%i\n", name, res.x, res.y, t);
            char buf[100] = {0};
            camera_type2str(t, buf, 100);
            printf("Camera %s: %ix%ix%i %s\n", name, res.x, res.y, t, buf);
            */


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
    Memory *mem = ecs_field(it, Memory, 2);
    Vec2i32 *res = ecs_field(it, Vec2i32, 3);
    for(int i = 0; i < it->count; ++i)
    {
        camera_read(cam + i, mem + i, res + i);
        //printf("Capture %s %ix%i %i %p\n", ecs_get_name(it->world, it->entities[i]), res[i].x, res[i].y, img[i].type, img[i].data);
    }
}











void EgVideoImport(ecs_world_t *world)
{
    ECS_MODULE(world, EgVideo);

    ECS_COMPONENT_DEFINE(world, Device);
    ECS_COMPONENT_DEFINE(world, Camera);

    ECS_SYSTEM(world, System_Camera_Capture, EcsOnUpdate, Camera, Memory, (Vec2i32, eg.types.Resolution));
    ECS_SYSTEM(world, System_Camera_Open, EcsOnUpdate, Device, Camera, (eg.types.Action, eg.types.Open));
    ECS_SYSTEM(world, System_Camera_Close, EcsOnUpdate, Camera, (eg.types.Action, eg.types.Close));
    ECS_OBSERVER(world, System_Camera_Create, EcsOnAdd, Camera);
    ECS_OBSERVER(world, System_Camera_Destroy, EcsOnRemove, Camera);

    ecs_struct(world, {
        .entity = ecs_id(Device),
        .members = {
            { .name = "path", .type = ecs_id(ecs_string_t) }
        }
    });

}