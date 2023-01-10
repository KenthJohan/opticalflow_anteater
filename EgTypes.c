#include "EgTypes.h"




ECS_DECLARE(Status);


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
ECS_DECLARE(Action);
ECS_DECLARE(Draw);
ECS_DECLARE(Visualize);
ECS_DECLARE(Window);
ECS_DECLARE(Copy);
ECS_DECLARE(CopyTo);
ECS_DECLARE(CopyFrom);
ECS_DECLARE(Area);
ECS_DECLARE(Snippet);
ECS_DECLARE(Capture);
ECS_DECLARE(Image);

ECS_COMPONENT_DECLARE(Vec2i32);
ECS_COMPONENT_DECLARE(Vec2f32);
ECS_COMPONENT_DECLARE(String);
ECS_COMPONENT_DECLARE(Channels);
ECS_COMPONENT_DECLARE(Mat);




ECS_CTOR(String, ptr, {
    ptr->value = NULL;
})
ECS_DTOR(String, ptr, {
    ecs_os_free(ptr->value);
})
ECS_MOVE(String, dst, src, {
    ecs_os_free(dst->value);
    dst->value = src->value;
    src->value = NULL;
})

ECS_COPY(String, dst, src, {
    ecs_os_free(dst->value);
    dst->value = ecs_os_strdup(src->value);
})

void hook_callback(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    ecs_entity_t event = it->event;
    for (int i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        //ecs_trace("%s: %s", ecs_get_name(world, event), ecs_get_name(world, e));
    }
}















void EgTypesImport(ecs_world_t *world)
{
    ECS_MODULE(world, EgTypes);
	
    ECS_ENTITY_DEFINE(world, Status, EcsUnion);
    ECS_ENTITY_DEFINE(world, Action, EcsUnion);
    ECS_ENTITY_DEFINE(world, Capture, EcsAcyclic);
    ECS_ENTITY_DEFINE(world, Uses, EcsAcyclic);
    ECS_ENTITY_DEFINE(world, Copy, EcsAcyclic);
    ECS_ENTITY_DEFINE(world, CopyTo, EcsAcyclic);
    ECS_ENTITY_DEFINE(world, CopyFrom, EcsAcyclic);
    ECS_ENTITY_DEFINE(world, Draw, EcsAcyclic);
    ECS_ENTITY_DEFINE(world, Image, EcsAcyclic);

    ECS_TAG_DEFINE(world, Resolution);
    ECS_TAG_DEFINE(world, Position);
    ECS_TAG_DEFINE(world, Velocity);
    ECS_TAG_DEFINE(world, CropPosition);
    ECS_TAG_DEFINE(world, CropSize);
    ECS_TAG_DEFINE(world, Open);
    ECS_TAG_DEFINE(world, OpenTry);
    ECS_TAG_DEFINE(world, OpenError);
    ECS_TAG_DEFINE(world, Close);
    ECS_TAG_DEFINE(world, CloseTry);
    ECS_TAG_DEFINE(world, CloseError);
    ECS_TAG_DEFINE(world, Draw);
    ECS_TAG_DEFINE(world, Visualize);
    ECS_TAG_DEFINE(world, Window);
    ECS_TAG_DEFINE(world, Area);
    ECS_TAG_DEFINE(world, Snippet);
    ECS_TAG_DEFINE(world, Image);




    ECS_COMPONENT_DEFINE(world, Vec2i32);
    ECS_COMPONENT_DEFINE(world, Vec2f32);
    ECS_COMPONENT_DEFINE(world, String);
    ECS_COMPONENT_DEFINE(world, Channels);
    ECS_COMPONENT_DEFINE(world, Mat);

    ecs_set_hooks(world, String, {
    .ctor = ecs_ctor(String),
    .move = ecs_move(String),
    .copy = ecs_copy(String),
    .dtor = ecs_dtor(String),
    .on_add = hook_callback,
    .on_remove = hook_callback,
    .on_set = hook_callback
    });

    ecs_struct(world, {
        .entity = ecs_id(String),
        .members = {
            { .name = "value", .type = ecs_id(ecs_string_t) }
        }
    });

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
        .entity = ecs_id(Channels),
        .members = {
            { .name = "n", .type = ecs_id(ecs_i32_t) }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Mat),
        .members = {
            { .name = "data", .type = ecs_id(ecs_uptr_t) },
            { .name = "data_size", .type = ecs_id(ecs_i32_t) },
            { .name = "type", .type = ecs_id(ecs_i32_t) },
            { .name = "dims", .type = ecs_id(ecs_i32_t) },
            { .name = "size", .type = ecs_id(ecs_i32_t), .count = 4 },
            { .name = "step", .type = ecs_id(ecs_i32_t), .count = 4 }
        }
    });

}