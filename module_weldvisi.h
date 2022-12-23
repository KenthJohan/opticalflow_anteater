#pragma once
#include "flecs.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif





extern ECS_DECLARE(Resolution);
extern ECS_DECLARE(Position);
extern ECS_DECLARE(Veclocity);
extern ECS_DECLARE(CropPosition);
extern ECS_DECLARE(CropSize);
extern ECS_COMPONENT_DECLARE(Weldvisi_View);
extern ECS_COMPONENT_DECLARE(Vec2i32);
extern ECS_COMPONENT_DECLARE(Vec2f32);

void SimpleModuleImport(ecs_world_t *world);

#ifdef __cplusplus
}
#endif