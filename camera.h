#pragma once
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CAMERA_PROP_FRAME_WIDTH 3 //!< Width of the frames in the video stream.
#define CAMERA_PROP_FRAME_HEIGHT 4 //!< Height of the frames in the video stream.
#define CAMERA_CAP_PROP_FORMAT 8 //!< Format of the %Mat objects (see Mat::type()) returned by VideoCapture::retrieve().

int camera_create(Camera * camera);
int camera_open(Camera * camera, char const * path);
int camera_close(Camera * camera);
int camera_destroy(Camera * camera);
int camera_read(Camera *camera, Memory * mem, Matspec * spec);
int camera_get_int(Camera *camera, int prop);
void camera_type2str(int type, char * buf, int len);




#ifdef __cplusplus
}
#endif