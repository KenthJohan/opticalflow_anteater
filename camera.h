#pragma once
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

int camera_create(Camera * camera);
int camera_open(Camera * camera, char const * path);
int camera_close(Camera * camera);
int camera_destroy(Camera * camera);
int camera_read(Camera * camera, Image * data, Vec2i32 * resolution, int * type);

#ifdef __cplusplus
}
#endif