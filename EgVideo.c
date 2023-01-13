#include "EgVideo.h"
#include "EgTypes.h"
#include "EgMemory.h"
#include "VideoReader.h"
#include <stdio.h>

ECS_COMPONENT_DECLARE(Device);
ECS_COMPONENT_DECLARE(VideoReader);








void System_Camera_Create(ecs_iter_t *it)
{
    printf("System_Camera_Create\n");
    VideoReader *c = ecs_field(it, VideoReader, 1);
    for(int i = 0; i < it->count; ++i)
    {
        printf("Create camera %s\n", ecs_get_name(it->world, it->entities[i]));
        VideoReader_create(c+i);
    }
}

void System_Camera_Destroy(ecs_iter_t *it)
{
    printf("System_Camera_Destroy\n");
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
            ecs_add_pair(it->world, it->entities[i], Status, OpenError);
            continue;
        }

        ecs_remove_pair(it->world, it->entities[i], Action, Open);
        ecs_add_pair(it->world, it->entities[i], Status, Open);
        
        /*
        res.x = VideoReader_get_int(c + i, VIDEOREADER_PROP_FRAME_WIDTHWIDTH);
        res.y = VideoReader_get_int(c + i, CAMERA_PROP_FRAME_HEIGHT);
        int t = VideoReader_get_int(c + i, CAMERA_CAP_PROP_FORMAT);
        */
        Mat mat;
        VideoReader_read(c + i, &mat);
        ecs_set_pair(it->world, it->entities[i], Vec2i32, Resolution, {mat.shape[1], mat.shape[0]});

        {
            char buf[100] = {0};
            cv_mat_type2str(mat.type, buf, 100);
            printf("VideoReader %s: %ix%ix%s\n", name, mat.shape[0], mat.shape[1], buf);
        }
    }
}



void System_Camera_Close(ecs_iter_t *it)
{
    printf("System_Camera_Close\n");
    VideoReader *c = ecs_field(it, VideoReader, 1);
    for(int i = 0; i < it->count; ++i)
    {
        int r = VideoReader_close(c + i);
        printf("VideoReader_close: %p, %i\n", c[i].handle, r);
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
    VideoReader *vid = ecs_field(it, VideoReader, 1);
    Mat *mat_field = ecs_field(it, Mat, 2);
    int mat_self = ecs_field_is_self(it, 2);
    for(int i = 0; i < it->count; ++i)
    {
        if (ecs_has_pair(it->world, it->entities[i], Status, Open) == false) {return;}
        Mat *m = mat_field + i * mat_self;
        //char const * name0 = ecs_get_name(it->world, e0);
        //char const * name = ecs_get_name(it->world, it->entities[i]);
        int r = VideoReader_read(vid, m);
        //printf("VideoReader_read: %s, %s, %i\n", name0, name, r);
        if(r != 0)
        {
            m[i].memory = NULL;
            m[i].start = NULL;
            m[i].size = 0;
            m[i].type = 0;
            m[i].dims = 0;
            m[i].shape[0] = 0;
            m[i].shape[1] = 0;
            m[i].shape[2] = 0;
            m[i].shape[3] = 0;
            m[i].step[0] = 0;
            m[i].step[1] = 0;
            m[i].step[2] = 0;
            m[i].step[3] = 0;
            //printf("Closing: %s %jx\n", name0, e0);
            ecs_add_pair(it->world, it->entities[i], Action, Close);
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
            {.id = ecs_id(VideoReader), .inout = EcsIn},
            {.id = ecs_id(Mat), .inout = EcsOut },
        },
        .callback = System_Camera_Capture
    });



    //ECS_SYSTEM(world, System_Camera_Capture, EcsOnUpdate, VideoReader(parent), (eg.types.Status(parent), eg.types.Open), Memory, Matspec, eg.types.Capture);
    ECS_SYSTEM(world, System_Camera_Open, EcsOnUpdate, Device, VideoReader, (eg.types.Action, eg.types.Open));
    ECS_SYSTEM(world, System_Camera_Close, EcsOnUpdate, VideoReader, (eg.types.Status, eg.types.Open), (eg.types.Action, eg.types.Close));
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
            { .name = "handle", .type = ecs_id(ecs_uptr_t) },
            { .name = "frame_offset", .type = ecs_id(ecs_i32_t) },
            { .name = "frame_count", .type = ecs_id(ecs_i32_t) }
        }
    });

}