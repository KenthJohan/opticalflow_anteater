#pragma once
#include "types.h"



void mat_roi(Mat const * src, Vec2i32 const* pos, Mat const* refdst, Mat * dst);
void mat_copy_region_auto_allocation(Mat const * src, Mat * dst, Vec2i32 const * pos, Vec2i32 const * area);