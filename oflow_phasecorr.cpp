#include "oflow.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>


using namespace cv;
using namespace std;

struct oflow_phasecorr_context
{
    Mat prvs;
};

void oflow_init(struct oflow_context * context, void * raw, int type, int rows, int cols)
{
    Mat cvraw = Mat(rows, cols, CV_8UC3, raw);
    context->internal = calloc(1, sizeof(oflow_phasecorr_context));
    oflow_phasecorr_context * internal = (oflow_phasecorr_context *)context->internal;
    cvtColor(cvraw, internal->prvs, COLOR_BGR2GRAY);
    internal->prvs.convertTo(internal->prvs, CV_32FC1, 1.0/255.0);
}


void oflow_run(struct oflow_context * context, void * raw, int type, int rows, int cols, float direction[2], float alpha, int cropx, int cropy, int cropw, int croph)
{
    oflow_phasecorr_context * internal = (oflow_phasecorr_context *)context->internal;
    Mat cvraw = Mat(rows, cols, CV_8UC3, raw)(Rect(cropx, cropy, cropw, croph));
    Mat next;
    cvtColor(cvraw, next, COLOR_BGR2GRAY);
    next.convertTo(next, CV_32FC1, 1.0/255.0);
    cv::Point2d d = phaseCorrelate(next, internal->prvs) * -1.0;
    // Direction average FIR calculation, FIR filter N=1
    direction[0] = direction[0] *(1.0f-alpha) + d.x * (alpha);
    direction[1] = direction[1] *(1.0f-alpha) + d.y * (alpha);

    printf("%6.2f %6.2f\n", direction[0], direction[1]);
    internal->prvs = next;
}