#pragma once
#include "flecs.h"
#include "types.h"



extern ECS_COMPONENT_DECLARE(Memory);
extern ECS_DECLARE(Image);

void EgMemoryImport(ecs_world_t *world);