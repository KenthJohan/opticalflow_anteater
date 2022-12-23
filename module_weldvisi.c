#include "module_weldvisi.h"
#include "oflow.h"
#include <stdio.h>

void Move(ecs_iter_t *it)
{
    Weldvisi_View *v = ecs_field(it, Weldvisi_View, 1);
    Vec2i32 *p = ecs_field(it, Vec2i32, 2);
    Vec2i32 *r = ecs_field(it, Vec2i32, 3);
    for(int i = 0; i < it->count; ++i)
    {
        printf("Move: %i %i\n", p[i].x, p[i].y);
        //void * context;
        //void * raw;
        //oflow_run(context, raw, 0, r[i], v + i, 0.1f, p[i], r[i]);
    }
}




ECS_DECLARE(Resolution);
ECS_DECLARE(Position);
ECS_DECLARE(Veclocity);
ECS_DECLARE(CropPosition);
ECS_DECLARE(CropSize);
ECS_COMPONENT_DECLARE(Weldvisi_View);
ECS_COMPONENT_DECLARE(Vec2i32);
ECS_COMPONENT_DECLARE(Vec2f32);

void SimpleModuleImport(ecs_world_t *world) {
    // Create the module entity. The PascalCase module name is translated to a
    // lower case path for the entity name, like "simple.module".
    ECS_MODULE(world, SimpleModule);

    // All contents of the module are created inside the module's namespace, so
    // the Position component will be created as simple.module.Position

    ECS_TAG_DEFINE(world, Resolution);
    ECS_TAG_DEFINE(world, Position);
    ECS_TAG_DEFINE(world, Veclocity);
    ECS_TAG_DEFINE(world, CropPosition);
    ECS_TAG_DEFINE(world, CropSize);
    ECS_COMPONENT_DEFINE(world, Weldvisi_View);
    ECS_COMPONENT_DEFINE(world, Vec2i32);
    ECS_COMPONENT_DEFINE(world, Vec2f32);

    ECS_SYSTEM(world, Move, EcsOnUpdate, Weldvisi_View, (CropPosition, Vec2i32), (CropSize, Vec2i32));
}