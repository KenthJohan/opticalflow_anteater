#pragma once
#include "types.h"

void mat_allocate(Tensor2_U8C3 * mat, int32_t w, int32_t h);
void mat_roi_fit(Tensor2_U8C3 * a, Tensor2_U8C3 * b);
void mat_roi_offset(Tensor2_U8C3 * a, Vec2i32 const* pos);
void mat_copy_region_auto_allocation(Tensor2_U8C3 const * src, Tensor2_U8C3 * dst, Vec2i32 const * pos, Vec2i32 const * area);