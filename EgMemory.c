#include "EgMemory.h"
#include "EgTypes.h"
#include <stdio.h>

ECS_COMPONENT_DECLARE(Memory);



ECS_CTOR(Memory, ptr, {
    printf("Memory Ctor\n");
    ecs_os_memset_t(ptr, 0, Memory);
})

ECS_DTOR(Memory, ptr, {
    printf("Memory Dtor\n");
})

ECS_MOVE(Memory, dst, src, {
    printf("Memory Move\n");
})

ECS_COPY(Memory, dst, src, {
    printf("Memory Copy\n");
})


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
    //printf("System_Memory_Copy %i\n", it->count);
    //https://docs.opencv.org/2.4/modules/core/doc/basic_structures.html#mat
    Memory *mem0 = ecs_field(it, Memory, 1); //Shared
    Matspec *spec0 = ecs_field(it, Matspec, 2); //Shared
    Memory *mem = ecs_field(it, Memory, 3);
    Matspec *spec = ecs_field(it, Matspec, 4);
    Vec2i32 *pos = ecs_field(it, Vec2i32, 5);
    Vec2i32 *area = ecs_field(it, Vec2i32, 6);
    if(mem0[0].data == NULL) {return;}
    for(int i = 0; i < it->count; ++i)
    {
        if(mem[i].data == NULL) {continue;}
        char const * name0 = ecs_get_name(it->world, ecs_field_src(it, 1));
        char const * name = ecs_get_name(it->world, it->entities[i]);
        int32_t reqsize = area[i].x * area[i].y * spec0[0].step[1];
        if(mem[i].size != reqsize)
        {
            printf("%s: Reqsize %i\n", name, reqsize);
            //ecs_os_free(mem[i].data);
            mem[i].data = ecs_os_malloc(reqsize);
            mem[i].size = reqsize;
        }
        printf("%s: Copyfrom %s\n", name, name0);
        spec[i].type = spec0[0].type;
        spec[i].dims = spec0[0].dims;
        spec[i].size[0] = area[i].y;
        spec[i].size[1] = area[i].x;
        spec[i].step[0] = area[i].x * spec0[0].step[1];
        spec[i].step[1] = spec0[0].step[1];
        copy1(mem[i].data, mem0[0].data, spec0[0].step, (int32_t[]){pos[i].y, pos[i].x}, (int32_t[]){area[i].y, area[i].x});
    }
}



void EgMemoryImport(ecs_world_t *world)
{
    ECS_MODULE(world, EgMemory);
    ECS_IMPORT(world, EgTypes);

    ECS_COMPONENT_DEFINE(world, Memory);
    ecs_set_hooks(world, Memory, {
        .ctor = ecs_ctor(Memory),
        //.move = ecs_move(Memory),
        //.copy = ecs_copy(Memory),
        //.dtor = ecs_dtor(Memory),
        //.on_add = System_Memory_callback,
        //.on_remove = System_Memory_callback,
        //.on_set = System_Memory_callback
    });

    ecs_struct(world, {
        .entity = ecs_id(Memory),
        .members = {
            { .name = "data", .type = ecs_id(ecs_uptr_t) },
            { .name = "size", .type = ecs_id(ecs_i32_t) }
        }
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .name = "System_Memory_Copy",
            .add = { ecs_dependson(EcsOnUpdate) }
        }),
        .query.filter.instanced = true,
        .query.filter.terms = {
            {.id = ecs_id(Memory), .inout = EcsIn, .src.flags = EcsUp, .src.trav = Copy},
            {.id = ecs_id(Matspec), .inout = EcsIn,.src.flags = EcsUp, .src.trav = Copy},
            {.id = ecs_id(Memory), .inout = EcsInOut },
            {.id = ecs_id(Matspec), .inout = EcsInOut },
            {.id = ecs_pair(ecs_id(Vec2i32), Position), .inout = EcsIn },
            {.id = ecs_pair(ecs_id(Vec2i32), Area), .inout = EcsIn }
        },
        .callback = System_Memory_Copy
    });
    


    //ECS_SYSTEM(world, System_Memory_Copy, EcsOnUpdate, Vec2i32(up(eg.types.Copy), eg.types.Resolution), Memory);

}