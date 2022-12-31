#pragma once
#include <stdint.h>


struct oflow_context
{
    void * internal;
};

typedef struct 
{
    int dummy;
    struct oflow_context context;
} Weldvisi_View;

typedef struct 
{
    int32_t x;
    int32_t y;
} Vec2i32;

typedef struct 
{
    int32_t n;
} Channels;

typedef struct 
{
    float x;
    float y;
} Vec2f32;

typedef struct 
{
    char * path;
} Device;

typedef struct  
{
    void * handle;
} Camera;

typedef struct  
{
    void * data;
    int32_t size;

    int32_t type;
    int32_t step;
} Memory;



typedef struct
{
    char *value;
} String;