#pragma once
#include "types.h"



void oflow_init(struct oflow_context * context, Mat* mat);
void oflow_run(struct oflow_context * context, Mat * mat, Vec2f32 * vel, float alpha);