#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include <opencv2/core/utils/filesystem.hpp>
#include <opencv2/core/types_c.h>
#include <stdio.h>
#include "oflow_farneback.hpp"
#include "common.h"
using namespace cv;



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

        {
            // Draw line from (c) to (cd):
            Point2f c = Point2f((float)flow.cols / 2.0f, (float)flow.rows / 2.0f);
            Point2f cd = c + direction_fir*-60.0f;
            arrowedLine(frame2, c, cd, Scalar(255, 255, 255), 2, LINE_4, 0, 0.5);
            char buf[100];

            // Print angle:
            //float angle = atan2(direction_fir.y, direction_fir.x);
            direction_fir;
            float speed = HYPOT_F32(direction_fir.x, direction_fir.y);
            //snprintf(buf, 100, "%f %f    ", da_fir.x, da_fir.y);
            snprintf(buf, 100, "%5.2f", speed);
            //snprintf(buf, 100, "%+5.0f    ", (angle / M_PI) * 180.0f);
            cv::putText(frame2, buf, cd, cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(0,255,0),2,false);
            printf("%s\n", buf);
        }
    }

}








void oflow_farneback_init(struct oflow_farneback_context * context, InputArray raw)
{
    cvtColor(raw, context->prvs, COLOR_BGR2GRAY);
}


void oflow_farneback_run(struct oflow_farneback_context * context, InputOutputArray raw)
{
    float alpha = 0.1f;

    Mat next;
    cvtColor(raw, next, COLOR_BGR2GRAY);
    go_with_the_flow(raw, next, context->prvs, alpha, context->direction);

    context->prvs = next;
}