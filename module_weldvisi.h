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
extern ECS_DECLARE(Uses);
extern ECS_DECLARE(Open);
extern ECS_DECLARE(OpenTry);
extern ECS_DECLARE(OpenError);
extern ECS_DECLARE(Close);
extern ECS_DECLARE(CloseTry);
extern ECS_DECLARE(CloseError);
extern ECS_DECLARE(Status);
extern ECS_DECLARE(Action);
extern ECS_DECLARE(Draw);
extern ECS_COMPONENT_DECLARE(Weldvisi_View);
extern ECS_COMPONENT_DECLARE(Vec2i32);
extern ECS_COMPONENT_DECLARE(Vec2f32);
extern ECS_COMPONENT_DECLARE(Device);
extern ECS_COMPONENT_DECLARE(Image);
extern ECS_COMPONENT_DECLARE(Camera);

void SimpleModuleImport(ecs_world_t *world);

#ifdef __cplusplus
}
#endif