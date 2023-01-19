#pragma once
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif


void draw_arrow(Tensor2_U8C3 * mat, Vec2i32 const * pos, Vec2i32 const * direction);
void draw_rectangle(Tensor2_U8C3 * mat, Vec2i32 const * pos, Vec2i32 const * length);
void draw_show(char const * title, Tensor2_U8C3 * mat);
void draw_weighed(Tensor2_U8C3 * mat1, double alpha, Tensor2_U8C3 * mat2, double beta, double gamma, Tensor2_U8C3 * dst);
void draw_weighed1(int32_t type, int32_t shape[], void * mat1, int32_t steps1[], double alpha, void * mat2, int32_t steps2[], double beta, double gamma, void * mat3, int32_t steps3[]);




#ifdef __cplusplus
}
#endif