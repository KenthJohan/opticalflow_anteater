#pragma once
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif


void motionest_phasecorr_init(struct oflow_context * context, Tensor2_U8C3* mat);
void motionest_phasecorr_run(struct oflow_context * context, Tensor2_U8C3 * mat, Vec2f32 * vel, float alpha);


#ifdef __cplusplus
}
#endif