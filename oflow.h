#pragma once
#include "types.h"



void oflow_init(struct oflow_context * context, void * raw, int type, Vec2i32 resolution);
void oflow_run(struct oflow_context * context, void * raw, int type, Vec2i32 resolution, Vec2f32 * vel, float alpha, Vec2i32 crop_pos, Vec2i32 crop_size);