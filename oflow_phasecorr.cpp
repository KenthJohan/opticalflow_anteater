#include "oflow.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>


using namespace cv;
using namespace std;

struct oflow_phasecorr_context
{
    Mat prvs;
};

void oflow_init(struct oflow_context * context, InputArray raw)
{
    context->internal = calloc(1, sizeof(oflow_phasecorr_context));
    oflow_phasecorr_context * internal = (oflow_phasecorr_context *)context->internal;
    cvtColor(raw, internal->prvs, COLOR_BGR2GRAY);
    internal->prvs.convertTo(internal->prvs,CV_32FC1, 1.0/255.0);
}


void oflow_run(struct oflow_context * context, InputOutputArray raw, Point2f& direction, float alpha)
{
    oflow_phasecorr_context * internal = (oflow_phasecorr_context *)context->internal;
    Mat next;
    cvtColor(raw, next, COLOR_BGR2GRAY);
    next.convertTo(next, CV_32FC1, 1.0/255.0);
    cv::Point2d d = phaseCorrelate(next, internal->prvs) * -1.0;
    // Direction average FIR calculation, FIR filter N=1
    direction.x = direction.x *(1.0f-alpha) + d.x * (alpha);
    direction.y = direction.y *(1.0f-alpha) + d.y * (alpha);

    printf("%6.2f %6.2f\n", direction.x, direction.y);
    internal->prvs = next;
}