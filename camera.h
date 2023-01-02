#pragma once
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CAMERA_PROP_FRAME_WIDTH 3 //!< Width of the frames in the video stream.
#define CAMERA_PROP_FRAME_HEIGHT 4 //!< Height of the frames in the video stream.
#define CAMERA_PROP_FORMAT 8 //!< Format of the %Mat objects (see Mat::type()) returned by VideoCapture::retrieve().

int camera_create(VideoReader * camera);
int camera_open(VideoReader * camera, char const * path);
int camera_close(VideoReader * camera);
int camera_destroy(VideoReader * camera);
int camera_read(VideoReader *camera, Memory * mem, Matspec * spec);
int camera_get_int(VideoReader *camera, int prop);
void camera_type2str(int type, char * buf, int len);




#ifdef __cplusplus
}
#endif