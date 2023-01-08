#include "EgVideo.h"
#include "EgTypes.h"
#include "EgMemory.h"
#include "VideoReader.h"
#include <stdio.h>

ECS_COMPONENT_DECLARE(Device);
ECS_COMPONENT_DECLARE(VideoReader);








void System_Camera_Create(ecs_iter_t *it)
{
    VideoReader *c = ecs_field(it, VideoReader, 1);
    for(int i = 0; i < it->count; ++i)
    {
        printf("Create camera %s\n", ecs_get_name(it->world, it->entities[i]));
        VideoReader_create(c+i);
    }
}

void System_Camera_Destroy(ecs_iter_t *it)
{
    VideoReader *c = ecs_field(it, VideoReader, 1);
    for(int i = 0; i < it->count; ++i)
    {
        printf("Destroy camera %s\n", ecs_get_name(it->world, it->entities[i]));
        VideoReader_destroy(c+i);
    }
}

void System_Camera_Open(ecs_iter_t *it)
{
    Device *d = ecs_field(it, Device, 1);
    VideoReader *c = ecs_field(it, VideoReader, 2);
    for(int i = 0; i < it->count; ++i)
    {
        char const * name = ecs_get_name(it->world, it->entities[i]);
        printf("Open camera %s: device: %s\n", name, d[i].path);
        int r = VideoReader_open(c + i, d[i].path);
        if(r != 0)
        {
            ecs_remove_pair(it->world, it->entities[i], Action, Open);
            ecs_add_pair(it->world, it->entities[i], ecs_id(Status), OpenError);
            continue;
        }

        ecs_remove_pair(it->world, it->entities[i], Action, Open);
        ecs_add_pair(it->world, it->entities[i], ecs_id(Status), Open);
        
        /*
        res.x = VideoReader_get_int(c + i, VIDEOREADER_PROP_FRAME_WIDTHWIDTH);
        res.y = VideoReader_get_int(c + i, CAMERA_PROP_FRAME_HEIGHT);
        int t = VideoReader_get_int(c + i, CAMERA_CAP_PROP_FORMAT);
        */
        Memory mem;
        Matspec spec;
        VideoReader_read(c + i, &mem, &spec);
        ecs_set_pair(it->world, it->entities[i], Vec2i32, Resolution, {spec.size[1], spec.size[0]});

        {
            char buf[100] = {0};
            cv_mat_type2str(spec.type, buf, 100);
            printf("VideoReader %s: %ix%ix%s\n", name, spec.size[0], spec.size[1], buf);
        }
    }
}



void System_Camera_Close(ecs_iter_t *it)
{
    Device *d = ecs_field(it, Device, 1);
    VideoReader *c = ecs_field(it, VideoReader, 2);
    for(int i = 0; i < it->count; ++i)
    {
        printf("Close camera: %s\n", d[i].path);
        int r = VideoReader_close(c + i);
        if(r == 0)
        {
            ecs_remove_pair(it->world, it->entities[i], Action, Close);
            ecs_add_pair(it->world, it->entities[i], ecs_id(Status), Close);
        }
        else
        {
            ecs_remove_pair(it->world, it->entities[i], Action, Close);
            ecs_add_pair(it->world, it->entities[i], ecs_id(Status), CloseError);
        }
    }
}



void System_Camera_Capture(ecs_iter_t *it)
{
    VideoReader *vid0 = ecs_field(it, VideoReader, 1); // Parent
    Memory *mem = ecs_field(it, Memory, 3);
    Matspec *spec = ecs_field(it, Matspec, 4);
    for(int i = 0; i < it->count; ++i)
    {
        char const * name0 = ecs_get_name(it->world, ecs_field_src(it, 1));
        char const * name = ecs_get_name(it->world, it->entities[i]);
        printf("VideoReader_read: %s, %s\n", name0, name);
        int r = VideoReader_read(vid0, mem + i, spec + i);
        if(r != 0)
        {
            ecs_add_pair(it->world, it->entities[i], Action, Close);
            ecs_add_pair(it->world, it->entities[i], ecs_id(Status), CloseTry);
        }
        //printf("Capture %s %ix%i %i %p\n", ecs_get_name(it->world, it->entities[i]), res[i].x, res[i].y, img[i].type, img[i].data);
    }
}











void EgVideoImport(ecs_world_t *world)
{
    ECS_MODULE(world, EgVideo);

    ECS_COMPONENT_DEFINE(world, Device);
    ECS_COMPONENT_DEFINE(world, VideoReader);


    ecs_system(world, {
        .entity = ecs_entity(world, {
            .name = "System_Camera_Capture",
            .add = { ecs_dependson(EcsOnUpdate) }
        }),
        .query.filter.instanced = true,
        .query.filter.terms = {
            {.id = ecs_id(VideoReader), .inout = EcsIn, .src.trav = EcsChildOf, .src.flags = EcsUp},
            {.id = ecs_pair(ecs_id(Status), Open), .inout = EcsIn, .src.trav = EcsChildOf, .src.flags = EcsUp},
            {.id = ecs_id(Memory), .inout = EcsInOut },
            {.id = ecs_id(Matspec), .inout = EcsInOut },
            {.id = ecs_id(Capture)}
        },
        .callback = System_Camera_Capture
    });



    //ECS_SYSTEM(world, System_Camera_Capture, EcsOnUpdate, VideoReader(parent), (eg.types.Status(parent), eg.types.Open), Memory, Matspec, eg.types.Capture);
    ECS_SYSTEM(world, System_Camera_Open, EcsOnUpdate, Device, VideoReader, (eg.types.Action, eg.types.Open));
    ECS_SYSTEM(world, System_Camera_Close, EcsOnUpdate, VideoReader, (eg.types.Action, eg.types.Close));
    ECS_OBSERVER(world, System_Camera_Create, EcsOnAdd, VideoReader);
    ECS_OBSERVER(world, System_Camera_Destroy, EcsOnRemove, VideoReader);

    ecs_struct(world, {
        .entity = ecs_id(Device),
        .members = {
            { .name = "path", .type = ecs_id(ecs_string_t) }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(VideoReader),
        .members = {
            { .name = "handle", .type = ecs_id(ecs_uptr_t) }
        }
    });

}