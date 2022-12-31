#include "EgMemory.h"
#include "EgTypes.h"
#include <stdio.h>

ECS_COMPONENT_DECLARE(Memory);
ECS_DECLARE(Image);



ECS_CTOR(Memory, ptr, {
    printf("Memory Ctor %i %i %ix%ix%ix%i\n", ptr->size, ptr->type, ptr->step[0], ptr->step[0], ptr->step[0], ptr->step[0]);
    ecs_os_memset_t(ptr, 0, Memory);
})

ECS_DTOR(Memory, ptr, {
    printf("Memory Dtor %i %i %i\n", ptr->size, ptr->type, ptr->step);
    ecs_os_free(ptr->data);
})

ECS_MOVE(Memory, dst, src, {
    printf("Memory Move %i %i %i\n", src->size, src->type, src->step);
    ecs_os_free(dst->data);
    dst->data = src->data;
    dst->size = src->size;
    dst->type = src->type;
    src->data = NULL;
})

ECS_COPY(Memory, dst, src, {
    printf("Memory Copy\n");
    ecs_os_memcpy_t(dst, src, Memory);
    //dst->data = ecs_os_memdup(src->data, src->size);
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

void System_Memory_Copy(ecs_iter_t *it)
{
    //https://docs.opencv.org/2.4/modules/core/doc/basic_structures.html#mat
    Memory *dst_mem = ecs_field(it, Memory, 1);
    Vec2i32 *dst_res = ecs_field(it, Vec2i32, 2);
    Memory *src_mem = ecs_field(it, Memory, 3); //Shared
    Vec2i32 *src_res = ecs_field(it, Vec2i32, 4); //Shared
    for(int i = 0; i < it->count; ++i)
    {
        printf("Copy %s %s\n", 
        ecs_get_name(it->world, it->entities[i]),
        ecs_get_name(it->world, ecs_field_src(it, 3))
        );
        if(dst_mem[i].size != src_mem[0].size)
        {
            ecs_os_free(dst_mem[i].data);
            dst_mem[i].data = ecs_os_malloc(src_mem[0].size);
            dst_mem[i].size = src_mem[0].size;
        }
        dst_mem[i].step[0] = src_mem[0].step[0];
        dst_mem[i].step[1] = src_mem[0].step[1];
        dst_mem[i].step[2] = src_mem[0].step[2];
        dst_mem[i].step[3] = src_mem[0].step[3];
        dst_mem[i].type = src_mem[0].type;
        ecs_os_memcpy(dst_mem[i].data, src_mem[0].data, src_mem[0].size);
        dst_res[i] = src_res[0];

        //ecs_remove_pair(it->world, it->entities[i], Copy, ecs_field_src(it, 1));
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
            { .name = "size", .type = ecs_id(ecs_i32_t) },

            { .name = "type", .type = ecs_id(ecs_i32_t) },
            { .name = "step", .type = ecs_id(ecs_i32_t), .count = 4 }
        }
    });

    ECS_PREFAB_DEFINE(world, Image, Memory, (Vec2i32, eg.types.Resolution));

    
    ecs_system(world, {
        .entity = ecs_entity(world, {
            .name = "System_Memory_Copy",
            .add = { ecs_dependson(EcsOnUpdate) }
        }),
        .query.filter.instanced = true,
        .query.filter.terms = {
            {.id = ecs_id(Memory), .inout = EcsIn },
            {.id = ecs_pair(ecs_id(Vec2i32), Resolution), .inout = EcsIn },
            {.id = ecs_id(Memory), .inout = EcsIn, .src.flags = EcsUp, .src.trav = Copy},
            {.id = ecs_pair(ecs_id(Vec2i32), Resolution), .inout = EcsIn,.src.flags = EcsUp, .src.trav = Copy}
        },
        .callback = System_Memory_Copy
    });


    //ECS_SYSTEM(world, System_Memory_Copy, EcsOnUpdate, Vec2i32(up(eg.types.Copy), eg.types.Resolution), Memory);

}