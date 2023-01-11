#pragma once
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif


void draw_arrow(void * image, int type, Vec2i32 resolution, Vec2i32 rio_pos, Vec2i32 rio_size, Vec2f32 direction, float gain);
void draw_rectangle(Mat * mat, Vec2i32 const * pos, Vec2i32 const * length);
void draw_show(char const * title, Mat * mat);
void draw_weighed(Mat * mat1, double alpha, Mat * mat2, double beta, double gamma, Mat * dst);
void draw_weighed1(int32_t type, int32_t shape[], void * mat1, int32_t steps1[], double alpha, void * mat2, int32_t steps2[], double beta, double gamma, void * mat3, int32_t steps3[]);




#ifdef __cplusplus
}
#endif