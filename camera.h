#pragma once
#include "types.h"


void camera_open();
void camera_close();
void camera_read(void * data, Vec2i32 * resolution, int * type);