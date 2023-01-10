#pragma once
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VIDEOREADER_PROP_FRAME_WIDTH 3 //!< Width of the frames in the video stream.
#define VIDEOREADER_PROP_FRAME_HEIGHT 4 //!< Height of the frames in the video stream.
#define VIDEOREADER_PROP_FORMAT 8 //!< Format of the %Mat objects (see Mat::type()) returned by VideoCapture::retrieve().
#define VIDEOREADER_PROP_POS_FRAMES 1



int VideoReader_create(VideoReader * camera);
int VideoReader_open(VideoReader * camera, char const * path);
int VideoReader_close(VideoReader * camera);
int VideoReader_destroy(VideoReader * camera);
int VideoReader_read(VideoReader *camera, Mat * mat);
int VideoReader_get_int(VideoReader *camera, int prop);
void cv_mat_type2str(int type, char * buf, int len);




#ifdef __cplusplus
}
#endif