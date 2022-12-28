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


void Image_callback(ecs_iter_t *it)
{
    ecs_world_t *world = it->world;
    ecs_entity_t event = it->event;

    for (int i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        printf("%s: %s\n", ecs_get_name(world, event), ecs_get_name(world, e));
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
        .on_add = Image_callback,
        .on_remove = Image_callback,
        .on_set = Image_callback
    });



}