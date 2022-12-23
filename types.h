#pragma once
#include <stdint.h>
#include "flecs.h"

typedef struct 
{
    int dummy;
} Weldvisi_View;

typedef struct 
{
    int32_t x;
    int32_t y;
} Vec2i32;

typedef struct 
{
    float x;
    float y;
} Vec2f32;

typedef struct 
{
    ecs_string_t path;
} Device;

typedef struct  
{
    void * handle;
} Camera;

typedef struct  
{
    void * data;
} Image;