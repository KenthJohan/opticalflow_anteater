#include "camera.h"
#include <opencv2/videoio.hpp>

struct Camera_CV
{
    cv::VideoCapture capture;
    cv::Mat frame;
};


int camera_create(Camera *camera)
{
    camera->handle = (void*) new Camera_CV;
    return 0;
}

int camera_open(Camera *camera, char const * path)
{
    if(camera->handle == NULL){return -1;}
    Camera_CV *c = (Camera_CV *)camera->handle;
    bool r = c->capture.open(path);
    return r == true ? 0 : -1;
}

int camera_close(Camera *camera)
{
    if(camera->handle == NULL){return -1;}
    Camera_CV *c = (Camera_CV *)camera->handle;
    c->capture.release();
    return 0;
}

int camera_destroy(Camera *camera)
{
    Camera_CV *c = (Camera_CV *)camera->handle;
    delete c;
    camera->handle = NULL;
    return 0;
}


void camera_type2str(int type, char * buf, int len)
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


int camera_read(Camera *camera, Memory * mem, Matspec * spec)
{
    Camera_CV *c = (Camera_CV *)camera->handle;
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

int camera_get_int(Camera *camera, int prop)
{
    Camera_CV *c = (Camera_CV *)camera->handle;
    return c->capture.get(prop);
}