#pragma once
#include <stdint.h>

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
    void * data;
    
} XMat;