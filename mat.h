#pragma once
#include "types.h"


void mat_roi_fit(Mat * a, Mat * b);
void mat_roi_offset(Mat * a, Vec2i32 const* pos);
void mat_copy_region_auto_allocation(Mat const * src, Mat * dst, Vec2i32 const * pos, Vec2i32 const * area);