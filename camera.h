#pragma once
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CAMERA_PROP_FRAME_WIDTH 3 //!< Width of the frames in the video stream.
#define CAMERA_PROP_FRAME_HEIGHT 4 //!< Height of the frames in the video stream.

int camera_create(Camera * camera);
int camera_open(Camera * camera, char const * path);
int camera_close(Camera * camera);
int camera_destroy(Camera * camera);
int camera_read(Camera * camera, Image * data, Vec2i32 * resolution, int * type);
int camera_get_int(Camera *camera, int prop);

#ifdef __cplusplus
}
#endif