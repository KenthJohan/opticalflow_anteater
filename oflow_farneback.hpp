#pragma once
#include <opencv2/core/types_c.h>
using namespace cv;


struct oflow_farneback_context
{
    Point2f direction;
    Mat raw;
    Mat prvs;
};



void oflow_farneback_init(struct oflow_farneback_context * context, InputArray raw);


void oflow_farneback_run(struct oflow_farneback_context * context, InputOutputArray raw);