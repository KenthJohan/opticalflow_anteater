#pragma once
#include "flecs.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif





extern ECS_COMPONENT_DECLARE(Weldvisi_View);

void SimpleModuleImport(ecs_world_t *world);

#ifdef __cplusplus
}
#endif