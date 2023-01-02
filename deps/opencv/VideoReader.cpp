#include "VideoReader.h"
#include <opencv2/videoio.hpp>

typedef struct
{
    cv::VideoCapture capture;
    cv::Mat frame;
} vidcapcv_t;


int VideoReader_create(VideoReader *camera)
{
    camera->handle = (void*) new vidcapcv_t;
    return 0;
}

int VideoReader_open(VideoReader *camera, char const * path)
{
    if(camera->handle == NULL){return -1;}
    vidcapcv_t *c = (vidcapcv_t *)camera->handle;
    bool r = c->capture.open(path);
    return r == true ? 0 : -1;
}

int VideoReader_close(VideoReader *camera)
{
    if(camera->handle == NULL){return -1;}
    vidcapcv_t *c = (vidcapcv_t *)camera->handle;
    c->capture.release();
    return 0;
}

int VideoReader_destroy(VideoReader *camera)
{
    vidcapcv_t *c = (vidcapcv_t *)camera->handle;
    delete c;
    camera->handle = NULL;
    return 0;
}


void cv_mat_type2str(int type, char * buf, int len)
{
  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar channels = 1 + (type >> CV_CN_SHIFT);
  char const * r;
  switch ( depth ) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
  }
  snprintf(buf, len, "%sC%i", r, channels);
}


int VideoReader_read(VideoReader *camera, Memory * mem, Matspec * spec)
{
    vidcapcv_t *c = (vidcapcv_t *)camera->handle;
    c->capture >> c->frame;
    if (c->frame.isContinuous() == false)
    {
        return -1;
    }
    mem->size = c->frame.step[0] * c->frame.rows;
    mem->data = c->frame.data;
    spec->dims = c->frame.dims;
    spec->type = c->frame.type();
    spec->step[0] = c->frame.step[0];
    spec->step[1] = c->frame.step[1];
    spec->size[0] = c->frame.size[0];
    spec->size[1] = c->frame.size[1];
    //printf("step: %i %i\n", spec->step[0], spec->step[1]);
    //printf("dim: %i %i\n", spec->dim[0], spec->dim[1]);
    return 0;
}

int VideoReader_get_int(VideoReader *camera, int prop)
{
    vidcapcv_t *c = (vidcapcv_t *)camera->handle;
    return c->capture.get(prop);
}