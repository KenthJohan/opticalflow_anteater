#include "mat.h"
#include "common.h"
#include "flecs.h"


// Region of interest:
// https://github.com/opencv/opencv/blob/9390c56831270a94af6480feb4cc330e4aa2ee5e/modules/core/src/matrix.cpp#L798
// https://github.com/opencv/opencv/blob/d9a444ca1a97740f9d092816a0ad0a523482911f/modules/core/include/opencv2/core/mat.hpp#L2622

void mat_allocate(Mat * mat, int32_t w, int32_t h)
{
    mat->type = 16; // OpenCV type: CV_U8C3
    mat->dims = 2; // Two axises

    mat->shape[0] = h;
    mat->shape[1] = w;

    mat->step[1] = 3; // Three bytes per pixel
    mat->step[0] = mat->shape[1] * mat->step[1]; // Bytes per row

    int32_t reqsize = mat->shape[0] * mat->shape[1] * mat->step[1];

    printf("Reqsize %i\n", reqsize);

    if(mat->size != reqsize)
    {
        ecs_os_free(mat->memory);
        mat->memory = ecs_os_malloc(reqsize);
        mat->start = mat->memory;
    }
    
    mat->size = reqsize;
}


void mat_roi_fit(Mat * a, Mat * b)
{
    a->shape[0] = EG_CLAMP(a->shape[0], 0, b->shape[0]);
    a->shape[1] = EG_CLAMP(a->shape[1], 0, b->shape[1]);
    b->shape[0] = EG_CLAMP(b->shape[0], 0, a->shape[0]);
    b->shape[1] = EG_CLAMP(b->shape[1], 0, a->shape[1]);
}

void mat_roi_offset(Mat * a, Vec2i32 const* pos)
{
    int32_t x = EG_CLAMP(pos->x, 0, a->shape[1]);
    int32_t y = EG_CLAMP(pos->y, 0, a->shape[0]);
    a->shape[1] = EG_CLAMP(a->shape[1] - x, 0, a->shape[1]);
    a->shape[0] = EG_CLAMP(a->shape[0] - y, 0, a->shape[0]);
    a->start += (y * a->step[0]) + (x * a->step[1]);
}


void copy1(uint8_t * dst, uint8_t const * src, int32_t const srcstep[2], int32_t const pos[2], int32_t const shape[2])
{
    //printf("%p %p, %i %i, %i %i, %i %i\n", dst, src, srcstep[0], srcstep[1], pos[0], pos[1], size[0], size[1]);
    src += pos[0]*srcstep[0] + pos[1]*srcstep[1];
    for(int32_t i = 0; i < shape[0]; ++i)
    {
        ecs_os_memcpy(dst, src, shape[1]*srcstep[1]);
        src += srcstep[0];
        dst += shape[1]*srcstep[1];
    }
}

void mat_copy_region_auto_allocation(Mat const * src, Mat * dst, Vec2i32 const * pos, Vec2i32 const * area)
{
    int32_t reqsize = area->x * area->y * src->step[1];
    if(dst->size != reqsize)
    {
        ecs_os_free(dst->memory);
        printf("Allocate %i\n", reqsize);
        dst->memory = ecs_os_malloc(reqsize);
        dst->start = dst->memory;
        dst->size = reqsize;
    }
    dst->type = src->type;
    dst->dims = src->dims;
    dst->shape[0] = area->y;
    dst->shape[1] = area->x;
    dst->step[0] = area->x * src->step[1];
    dst->step[1] = src->step[1];
    copy1(dst->start, src->start, src->step, (int32_t[]){pos->y, pos->x}, (int32_t[]){area->y, area->x});
}

