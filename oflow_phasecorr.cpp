#include "oflow.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>


using namespace cv;
using namespace std;

struct oflow_phasecorr_context
{
    Mat prvs;
};


void oflow_init(struct oflow_context * context, void * raw, int type, Vec2i32 resolution)
{
    Mat cvraw = Mat(resolution.y, resolution.x, CV_8UC3, raw);
    context->internal = calloc(1, sizeof(oflow_phasecorr_context));
    oflow_phasecorr_context * internal = (oflow_phasecorr_context *)context->internal;
    cvtColor(cvraw, internal->prvs, COLOR_BGR2GRAY);
    internal->prvs.convertTo(internal->prvs, CV_32FC1, 1.0/255.0);
}


void oflow_run(struct oflow_context * context, void * raw, int type, Vec2i32 resolution, Vec2f32 * vel, float alpha, Vec2i32 crop_pos, Vec2i32 crop_size)
{
    oflow_phasecorr_context * internal = (oflow_phasecorr_context *)context->internal;
    Mat cvraw = Mat(resolution.y, resolution.x, CV_8UC3, raw)(Rect(crop_pos.x, crop_pos.y, crop_size.x, crop_size.y));
    Mat next;
    cvtColor(cvraw, next, COLOR_BGR2GRAY);
    next.convertTo(next, CV_32FC1, 1.0/255.0);
    cv::Point2d d = phaseCorrelate(next, internal->prvs) * -1.0;
    // Direction average FIR calculation, FIR filter N=1
    vel[0].x = vel[0].x *(1.0f-alpha) + d.x * (alpha);
    vel[0].y = vel[0].y *(1.0f-alpha) + d.y * (alpha);
    //printf("%6.2f %6.2f\n", vel[0].x, vel[0].y);
    internal->prvs = next;
}