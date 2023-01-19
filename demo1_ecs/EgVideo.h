#pragma once
#include "flecs.h"
#include "types.h"




extern ECS_COMPONENT_DECLARE(Device);
extern ECS_COMPONENT_DECLARE(VideoReader);


void EgVideoImport(ecs_world_t *world);