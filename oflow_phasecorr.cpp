#include "oflow.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>



struct oflow_phasecorr_context
{
    cv::Mat prvs;
};


void oflow_init(struct oflow_context * context, void * raw, int type, Vec2i32 resolution)
{
    cv::Mat cvraw = cv::Mat(resolution.y, resolution.x, CV_8UC3, raw);
    context->internal = new oflow_phasecorr_context;
    oflow_phasecorr_context * internal = (oflow_phasecorr_context *)context->internal;
    cvtColor(cvraw, internal->prvs, cv::COLOR_BGR2GRAY);
    internal->prvs.convertTo(internal->prvs, CV_32FC1, 1.0/255.0);
}


void oflow_run(struct oflow_context * context, void * raw, int type, Vec2i32 resolution, Vec2f32 * vel, float alpha, Vec2i32 crop_pos, Vec2i32 crop_size)
{
    oflow_phasecorr_context * internal = (oflow_phasecorr_context *)context->internal;
    cv::Rect rio = cv::Rect(crop_pos.x, crop_pos.y, crop_size.x, crop_size.y);
    cv::Mat cvraw = cv::Mat(resolution.y, resolution.x, type, raw)(rio);
    cv::Mat next;
    cv::cvtColor(cvraw, next, cv::COLOR_BGR2GRAY);
    next.convertTo(next, CV_32FC1, 1.0/255.0);
    cv::Point2d d = phaseCorrelate(next, internal->prvs) * -1.0;
    // Direction average FIR calculation, FIR filter N=1
    vel[0].x = vel[0].x *(1.0f-alpha) + d.x * (alpha);
    vel[0].y = vel[0].y *(1.0f-alpha) + d.y * (alpha);
    //printf("%6.2f %6.2f\n", vel[0].x, vel[0].y);
    internal->prvs = next;
}