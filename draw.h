#pragma once
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif


void draw_arrow(void * image, int type, Vec2i32 resolution, Vec2i32 rio_pos, Vec2i32 rio_size, Vec2f32 direction, float gain);
void draw_rectangle(void * image, int type, Vec2i32 resolution, Vec2i32 pos, Vec2i32 length);
void draw_show(char const * title, Mat * mat);
void draw_weighed(Mat * mat1, double alpha, Mat * mat2, double beta, double gamma, Mat * dst);





#ifdef __cplusplus
}
#endif