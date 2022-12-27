#include "module_weldvisi.h"
#include "oflow.h"
#include "camera.h"
#include "draw.h"
#include <stdio.h>


void System_Test(ecs_iter_t *it)
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


void System_Motion_Estimation(ecs_iter_t *it)
{
    printf("System_Motion_Estimation %i\n", it->count);
    Weldvisi_View *v = ecs_field(it, Weldvisi_View, 1);
    Vec2i32 *rio_pos = ecs_field(it, Vec2i32, 2);
    Vec2i32 *rio_len = ecs_field(it, Vec2i32, 3);
    Vec2i32 *vel = ecs_field(it, Vec2i32, 4);
    Image *img = ecs_field(it, Image, 5); // Shared
    Vec2i32 *res = ecs_field(it, Vec2i32, 6);  // Shared
    for(int i = 0; i < it->count; ++i)
    {
        printf("%s:\n", ecs_get_name(it->world, it->entities[i]));
        //printf("%s: %i %i\n", ecs_get_name(it->world, it->entities[i]), res[i].x, res[i].y);
        float alpha = 0.1f;
        //oflow_run(&v[i].context, img[0].data, img[0].type, res[0], vel + i, alpha, rio_pos[i], rio_len[i]);
    }
}


void System_Olfow_Create(ecs_iter_t *it)
{
    Camera *c = ecs_field(it, Camera, 1);
    for(int i = 0; i < it->count; ++i)
    {
        printf("Create camera %s\n", ecs_get_name(it->world, it->entities[i]));
        camera_create(c+i);
    }
}

void System_Olfow_Destroy(ecs_iter_t *it)
{
    Camera *c = ecs_field(it, Camera, 1);
    for(int i = 0; i < it->count; ++i)
    {
        printf("Destroy camera %s\n", ecs_get_name(it->world, it->entities[i]));
        camera_destroy(c+i);
    }
}





void System_Draw(ecs_iter_t *it)
{
    Image *img = ecs_field(it, Image, 1);
    Vec2i32 *res = ecs_field(it, Vec2i32, 2);
    for(int i = 0; i < it->count; ++i)
    {
        draw_show(img[i].data, img[i].type, res[i]);
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
ECS_DECLARE(Velocity);
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
ECS_DECLARE(Draw);
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
    ECS_TAG_DEFINE(world, Velocity);
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
    ECS_TAG_DEFINE(world, Draw);
    ECS_COMPONENT_DEFINE(world, Weldvisi_View);
    ECS_COMPONENT_DEFINE(world, Vec2i32);
    ECS_COMPONENT_DEFINE(world, Vec2f32);
    ECS_COMPONENT_DEFINE(world, Device);
    ECS_COMPONENT_DEFINE(world, Image);
    ECS_COMPONENT_DEFINE(world, Camera);


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
            {.id = ecs_id(Image), .inout = EcsIn, .src.flags = EcsUp, .src.trav = Uses},
            {.id = ecs_pair(ecs_id(Vec2i32), Resolution), .inout = EcsIn,.src.flags = EcsUp,.src.trav = Uses}
        },
        .callback = System_Motion_Estimation
    });

    
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

    ECS_SYSTEM(world, System_Draw, EcsOnUpdate, Image, (Vec2i32, Resolution), Draw);
    
    ECS_SYSTEM(world, System_Camera_Capture, EcsOnUpdate, Camera, Image, (Vec2i32, Resolution));
    ECS_SYSTEM(world, System_Camera_Open, EcsOnUpdate, Device, Camera, (Action, Open));
    ECS_SYSTEM(world, System_Camera_Close, EcsOnUpdate, Camera, (Action, Close));
    ECS_OBSERVER(world, System_Camera_Create, EcsOnAdd, Camera);
    ECS_OBSERVER(world, System_Camera_Destroy, EcsOnRemove, Camera);

    ECS_OBSERVER(world, System_Oflow_Create, EcsOnAdd, Weldvisi_View);
    ECS_OBSERVER(world, System_Oflow_Destroy, EcsOnRemove, Weldvisi_View);


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