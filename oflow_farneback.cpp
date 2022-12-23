#include "oflow.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>

using namespace cv;


struct oflow_farneback_context
{
    Mat prvs;
};


void go_with_the_flow(InputOutputArray frame2, Mat next, Mat prvs, float alpha, Point2f& direction_fir)
{
    Mat flow(prvs.size(), CV_32FC2);
    double pyr_scale = 0.5;
    int levels = 3;
    int winsize = 15;
    int iterations = 3;
    int poly_n = 7;
    double poly_sigma = 1.2;
    int flags = 0;
    
    //printf("rows:%i cols:%i\n", next.rows, next.cols);
    calcOpticalFlowFarneback(prvs, next, flow, pyr_scale, levels, winsize, iterations, poly_n, poly_sigma, flags);
    // visualization
    Mat flowp[2];
    split(flow, flowp);
    Mat magnitude, angle, magn_norm;
    cartToPolar(flowp[0], flowp[1], magnitude, angle, true);
    normalize(magnitude, magn_norm, 0.0f, 1.0f, NORM_MINMAX);
    angle *= ((1.f / 360.f) * (180.f / 255.f));
    //build hsv image
    Mat _hsv[3], hsv, hsv8, bgr;
    _hsv[0] = angle;
    _hsv[1] = Mat::ones(angle.size(), CV_32F);
    _hsv[2] = magn_norm;
    merge(_hsv, 3, hsv);
    hsv.convertTo(hsv8, CV_8U, 255.0);
    cvtColor(hsv8, bgr, COLOR_HSV2BGR);
    
    
    
    //Mat overlay;
    addWeighted(bgr, 0.5, frame2, 0.5, 0.0, frame2);
    {
        // Direction average calculation:
        Point2f direction = Point2f(0,0);
        for(int j = 0;j < flow.cols;j++){
            for(int i = 0;i < flow.rows;i++){
                direction.x += flowp[0].at<float>(i,j);
                direction.y += flowp[1].at<float>(i,j);
            }
        }
        direction.x *= (1.0f/(flow.cols*flow.rows));
        direction.y *= (1.0f/(flow.cols*flow.rows));
        
        // Direction average FIR calculation, FIR filter N=1
        direction_fir.x = direction_fir.x *(1.0f-alpha) + direction.x * (alpha);
        direction_fir.y = direction_fir.y *(1.0f-alpha) + direction.y * (alpha);
    }

}





void oflow_init(struct oflow_context * context, InputArray raw)
{
    context->internal = calloc(1, sizeof(oflow_farneback_context));
    oflow_farneback_context * internal = (oflow_farneback_context *)context->internal;
    cvtColor(raw, internal->prvs, COLOR_BGR2GRAY);
}


void oflow_run(struct oflow_context * context, InputOutputArray raw, Point2f& direction, float alpha)
{
    oflow_farneback_context * internal = (oflow_farneback_context *)context->internal;
    Mat next;
    cvtColor(raw, next, COLOR_BGR2GRAY);
    go_with_the_flow(raw, next, internal->prvs, alpha, direction);
    internal->prvs = next;
}