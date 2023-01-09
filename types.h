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
    int32_t frame_offset;
    int32_t frame_count;
} VideoReader;

typedef struct  
{
    uint8_t * data;
    int32_t size;
} Memory;


typedef struct  
{
    int32_t type;
    int32_t dims;
    int32_t size[4];
    int32_t step[4];
} Matspec;



typedef struct
{
    char *value;
} String;