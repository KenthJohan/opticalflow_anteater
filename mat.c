#include "mat.h"
#include "common.h"
#include "flecs.h"


// Region of interest:
// https://github.com/opencv/opencv/blob/9390c56831270a94af6480feb4cc330e4aa2ee5e/modules/core/src/matrix.cpp#L798
// https://github.com/opencv/opencv/blob/d9a444ca1a97740f9d092816a0ad0a523482911f/modules/core/include/opencv2/core/mat.hpp#L2622
void mat_roi(Mat const * src, Vec2i32 const* pos, Mat const* refdst, Mat * dst)
{
    int32_t x = EG_CLAMP(pos->x, 0, refdst->shape[1] - src->shape[1]);
    int32_t y = EG_CLAMP(pos->y, 0, refdst->shape[0] - src->shape[0]);
    dst->start = refdst->start + (y * refdst->step[0]) + (x * refdst->step[1]);
    dst->shape[0] = src->shape[0];
    dst->shape[1] = src->shape[1];
	dst->dims = refdst->dims;
	dst->type = refdst->type;
	dst->step[0] = refdst->step[0];
	dst->step[1] = refdst->step[1];
	dst->memory = NULL;
	dst->size = 0;
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
        //printf("%s: Reqsize %i\n", name, reqsize);
        ecs_os_free(dst->memory);
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

