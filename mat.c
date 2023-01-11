#include "mat.h"
#include "common.h"


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
