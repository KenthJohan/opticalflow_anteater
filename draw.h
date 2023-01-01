#pragma once
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif


void draw_arrow(void * image, int type, Vec2i32 resolution, Vec2i32 rio_pos, Vec2i32 rio_size, Vec2f32 direction, float gain);
void draw_rectangle(void * image, int type, Vec2i32 resolution, Vec2i32 pos, Vec2i32 length);
void draw_show(char const * title, void * image, Matspec * spec);





#ifdef __cplusplus
}
#endif