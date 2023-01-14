#pragma once
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif


void oflow_init(struct oflow_context * context, Mat* mat);
void oflow_run(struct oflow_context * context, Mat * mat, Vec2f32 * vel, float alpha);


#ifdef __cplusplus
}
#endif