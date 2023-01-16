#include "EgVideo.h"
#include "EgTypes.h"
#include "EgMats.h"
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
        ecs_add_pair(it->world, it->entities[i], Status, Close);
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
            printf("VideoReader_open Error\n");
            //ecs_remove_pair(it->world, it->entities[i], Action, Open);
            ecs_add_pair(it->world, it->entities[i], Action, Action);
            ecs_add_pair(it->world, it->entities[i], Status, OpenError);
            continue;
        }

        
        //TODO: Why is removing pair causing (fatal: flecs.c: 5884: assert: tr != NULL INTERNAL_ERROR) ?
        //ecs_remove_pair(it->world, it->entities[i], Action, Open);
        ecs_add_pair(it->world, it->entities[i], Action, Action);
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
            //TODO: Why is removing pair causing (fatal: flecs.c: 5884: assert: tr != NULL INTERNAL_ERROR) ?
            //ecs_remove_pair(it->world, it->entities[i], Action, Close);
            ecs_add_pair(it->world, it->entities[i], Action, Action);
            ecs_add_pair(it->world, it->entities[i], Status, Close);
        }
        else
        {
            //ecs_remove_pair(it->world, it->entities[i], Action, Close);
            ecs_remove_pair(it->world, it->entities[i], Action, Action);
            ecs_add_pair(it->world, it->entities[i], Status, CloseError);
        }
    }
}



void System_Camera_Reader(ecs_iter_t *it)
{
    VideoReader *vid_field = ecs_field(it, VideoReader, 1);
    Mat         *mat_field = ecs_field(it, Mat, 2);
    int          mat_self = ecs_field_is_self(it, 2);
    for(int i = 0; i < it->count; ++i)
    {
        if (ecs_has_pair(it->world, it->entities[i], Status, Open) == false) {return;}
        VideoReader *v = vid_field + i;
        Mat         *m = mat_field + i * mat_self;
        //char const * name0 = ecs_get_name(it->world, e0);
        //char const * name = ecs_get_name(it->world, it->entities[i]);
        int r = VideoReader_read(v, m);
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
            .name = "System_Camera_Reader",
            .add = { ecs_dependson(EcsOnUpdate) }
        }),
        .query.filter.instanced = true,
        .query.filter.terms = {
            {.id = ecs_id(VideoReader), .inout = EcsIn},
            {.id = ecs_id(Mat), .inout = EcsOut },
        },
        .callback = System_Camera_Reader
    });

    ECS_SYSTEM(world, System_Camera_Open, EcsOnUpdate, Device, VideoReader, (eg.types.Status, eg.types.Close), (eg.types.Action, eg.types.Open));
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