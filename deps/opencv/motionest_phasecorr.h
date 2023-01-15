#pragma once
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif


void motionest_phasecorr_init(struct oflow_context * context, Mat* mat);
void motionest_phasecorr_run(struct oflow_context * context, Mat * mat, Vec2f32 * vel, float alpha);


#ifdef __cplusplus
}
#endif