#include "module_weldvisi.h"


void Move(ecs_iter_t *it)
{
    Weldvisi_View *p = ecs_field(it, Weldvisi_View, 1);
    Rectangle_i32 *v = ecs_field(it, Rectangle_i32, 2);
}




ECS_COMPONENT_DECLARE(Weldvisi_View);
ECS_COMPONENT_DECLARE(Rectangle_i32);
ECS_COMPONENT_DECLARE(Velocity_f32);

void SimpleModuleImport(ecs_world_t *world) {
    // Create the module entity. The PascalCase module name is translated to a
    // lower case path for the entity name, like "simple.module".
    ECS_MODULE(world, SimpleModule);

    // All contents of the module are created inside the module's namespace, so
    // the Position component will be created as simple.module.Position

    ECS_COMPONENT_DEFINE(world, Weldvisi_View);
    ECS_COMPONENT_DEFINE(world, Rectangle_i32);
    ECS_COMPONENT_DEFINE(world, Velocity_f32);

    ECS_SYSTEM(world, Move, EcsOnUpdate, Weldvisi_View, Rectangle_i32);
}