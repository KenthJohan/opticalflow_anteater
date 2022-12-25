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

int camera_read(Camera *camera, void * data, Vec2i32 * resolution, int * type)
{
    Camera_CV *c = (Camera_CV *)camera->handle;
    c->capture >> c->frame;
    return 0;
}

int camera_get_int(Camera *camera, int prop)
{
    Camera_CV *c = (Camera_CV *)camera->handle;
    return c->capture.get(prop);
}