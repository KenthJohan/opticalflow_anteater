#include "EgMemory.h"
#include "EgTypes.h"

ECS_COMPONENT_DECLARE(Image);




ECS_CTOR(Image, ptr, {
    printf("Ctor\n");
    ptr->data = NULL;
})

ECS_DTOR(Image, ptr, {
    printf("Dtor\n");
})

ECS_MOVE(Image, dst, src, {
    printf("Move\n");
})

ECS_COPY(Image, dst, src, {
    printf("Copy\n");
})


void System_Image_callback(ecs_iter_t *it)
{
    ecs_world_t *world = it->world;
    ecs_entity_t event = it->event;

    for (int i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        printf("%s: %s\n", ecs_get_name(world, event), ecs_get_name(world, e));
    }
}

void System_Image_Copy(ecs_iter_t *it)
{
    Image *img1 = ecs_field(it, Image, 1); // Shared
    Vec2i32 *res1 = ecs_field(it, Vec2i32, 2); // Shared
    Image *img2 = ecs_field(it, Image, 3);
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
    ECS_COMPONENT_DEFINE(world, Image);


    ecs_set_hooks(world, Image, {
        .ctor = ecs_ctor(Image),
        .move = ecs_move(Image),
        .copy = ecs_copy(Image),
        .dtor = ecs_dtor(Image),
        .on_add = System_Image_callback,
        .on_remove = System_Image_callback,
        .on_set = System_Image_callback
    });

    


    ECS_SYSTEM(world, System_Image_Copy, EcsOnUpdate, 
        Image, 
        (eg.types.Vec2i32, eg.types.Resolution), 
        Image(up(eg.types.Copy)), 
        Vec2i32(up(eg.types.Copy), eg.types.Resolution)
    );

}