#pragma once
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif


void motionest_lucaskanade_init(struct oflow_context * context, Tensor2_U8C3* mat);
void motionest_lucaskanade_run(struct oflow_context * context, Tensor2_U8C3 * mat, Vec2f32 * vel, float alpha);


#ifdef __cplusplus
}
#endif