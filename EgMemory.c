#include "EgMemory.h"
#include "EgTypes.h"

ECS_COMPONENT_DECLARE(Memory);




ECS_CTOR(Memory, ptr, {
    printf("Memory Ctor\n");
    ptr->data = NULL;
})

ECS_DTOR(Memory, ptr, {
    printf("Memory Dtor\n");
    ecs_os_free(ptr->data);
})

ECS_MOVE(Memory, dst, src, {
    printf("Memory Move\n");
    ecs_os_free(dst->data);
    dst->data = src->data;
    src->data = NULL; // This makes sure the value doesn't get deleted twice,
                       // as the destructor is still invoked after a move.
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

void System_Memory_Copy(ecs_iter_t *it)
{
    Memory *img1 = ecs_field(it, Memory, 1); // Shared
    Vec2i32 *res1 = ecs_field(it, Vec2i32, 2); // Shared
    Memory *img2 = ecs_field(it, Memory, 3);
    Vec2i32 *res2 = ecs_field(it, Vec2i32, 4);
    for(int i = 0; i < it->count; ++i)
    {
        printf("Copy %s %s\n", 
        ecs_get_name(it->world, it->entities[i]),
        ecs_get_name(it->world, ecs_field_src(it, 3))
        );
        ecs_remove_pair(it->world, it->entities[i], Copy, ecs_field_src(it, 3));
    }
}


void EgMemoryImport(ecs_world_t *world)
{
    ECS_MODULE(world, EgMemory);
    ECS_IMPORT(world, EgTypes);
    ECS_COMPONENT_DEFINE(world, Memory);


    ecs_set_hooks(world, Memory, {
        .ctor = ecs_ctor(Memory),
        .move = ecs_move(Memory),
        .copy = ecs_copy(Memory),
        .dtor = ecs_dtor(Memory),
        .on_add = System_Memory_callback,
        .on_remove = System_Memory_callback,
        .on_set = System_Memory_callback
    });




    ecs_struct(world, {
        .entity = ecs_id(Memory),
        .members = {
            { .name = "data", .type = ecs_id(ecs_uptr_t) },
            { .name = "type", .type = ecs_id(ecs_i32_t) },
            { .name = "step", .type = ecs_id(ecs_i32_t) },
            { .name = "size", .type = ecs_id(ecs_i32_t) }
        }
    });


    ECS_SYSTEM(world, System_Memory_Copy, EcsOnUpdate, 
        Memory, 
        (eg.types.Vec2i32, eg.types.Resolution), 
        Memory(up(eg.types.Copy)), 
        Vec2i32(up(eg.types.Copy), eg.types.Resolution)
    );

}