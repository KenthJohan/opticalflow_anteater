#pragma once
#include "flecs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct 
{
    int dummy;
} Weldvisi_View;

typedef struct 
{
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
} Rectangle_i32;

typedef struct  
{
    float x;
    float y;
} Velocity_f32;

typedef struct  
{
    void * data;
    
} XMat;



extern ECS_COMPONENT_DECLARE(Weldvisi_View);
extern ECS_COMPONENT_DECLARE(Rectangle_i32);
extern ECS_COMPONENT_DECLARE(Velocity_f32);

void SimpleModuleImport(ecs_world_t *world);

#ifdef __cplusplus
}
#endif