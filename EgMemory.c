#include "EgMemory.h"
#include "EgTypes.h"
#include <stdio.h>




void System_Memory_callback(ecs_iter_t *it)
{
    ecs_world_t *world = it->world;
    ecs_entity_t event = it->event;

    for (int i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        printf("%s: %s\n", ecs_get_name(world, event), ecs_get_name(world, e));
    }
}



void copy1(uint8_t * dst, uint8_t * src, int32_t srcstep[2], int32_t pos[2], int32_t size[2])
{
    printf("%p %p, %i %i, %i %i, %i %i\n", dst, src, srcstep[0], srcstep[1], pos[0], pos[1], size[0], size[1]);
    src += pos[0]*srcstep[0] + pos[1]*srcstep[1];
    for(int32_t i = 0; i < size[0]; ++i)
    {
        ecs_os_memcpy(dst, src, size[1]*srcstep[1]);
        src += srcstep[0];
        dst += size[1]*srcstep[1];
    }
}




void System_Memory_Copy(ecs_iter_t *it)
{
    //https://docs.opencv.org/2.4/modules/core/doc/basic_structures.html#mat
    Mat *mat0 = ecs_field(it, Mat, 1); //Shared
    Mat *mat = ecs_field(it, Mat, 2);
    Vec2i32 *pos = ecs_field(it, Vec2i32, 3);
    Vec2i32 *area = ecs_field(it, Vec2i32, 4);
    if(mat0->data == NULL) {return;}
    for(int i = 0; i < it->count; ++i)
    {
        char const * name0 = ecs_get_name(it->world, ecs_field_src(it, 1));
        char const * name = ecs_get_name(it->world, it->entities[i]);
        int32_t reqsize = area[i].x * area[i].y * mat0->step[1];
        if(mat[i].size != reqsize)
        {
            printf("%s: Reqsize %i\n", name, reqsize);
            //ecs_os_free(mem[i].data);
            mat[i].data = ecs_os_malloc(reqsize);
            mat[i].data_size = reqsize;
        }
        printf("%s: Copyfrom %s\n", name, name0);
        mat[i].type = mat0[0].type;
        mat[i].dims = mat0[0].dims;
        mat[i].size[0] = area[i].y;
        mat[i].size[1] = area[i].x;
        mat[i].step[0] = area[i].x * mat0->step[1];
        mat[i].step[1] = mat0->step[1];
        copy1(mat[i].data, mat0->data, mat0->step, (int32_t[]){pos[i].y, pos[i].x}, (int32_t[]){area[i].y, area[i].x});
    }
}

void Observer_Mat_EcsOnSet(ecs_iter_t *it)
{
    Mat *mat = ecs_field(it, Mat, 1);
    for(int i = 0; i < it->count; ++i)
    {
        char const * name = ecs_get_name(it->world, it->entities[i]);
        printf("Observer_Mat_EcsOnSet: %s\n", name);
    }
}

void EgMemoryImport(ecs_world_t *world)
{
    ECS_MODULE(world, EgMemory);
    ECS_IMPORT(world, EgTypes);


    ecs_system(world, {
        .entity = ecs_entity(world, {
            .name = "System_Memory_Copy",
            .add = { ecs_dependson(EcsOnUpdate) }
        }),
        .query.filter.instanced = true,
        .query.filter.terms = {
            {.id = ecs_id(Mat), .inout = EcsIn, .src.flags = EcsUp, .src.trav = Copy},
            {.id = ecs_id(Mat), .inout = EcsInOut },
            {.id = ecs_pair(ecs_id(Vec2i32), Position), .inout = EcsIn },
            {.id = ecs_pair(ecs_id(Vec2i32), Area), .inout = EcsIn }
        },
        .callback = System_Memory_Copy
    });
    
    ECS_OBSERVER(world, Observer_Mat_EcsOnSet, EcsOnSet, Mat);

    //ECS_SYSTEM(world, System_Memory_Copy, EcsOnUpdate, Vec2i32(up(eg.types.Copy), eg.types.Resolution), Memory);

}