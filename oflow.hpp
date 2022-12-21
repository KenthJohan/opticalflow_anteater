#pragma once
#include <opencv2/core/types_c.h>

struct oflow_context
{
    void * internal;
};

void oflow_init(struct oflow_context * context, cv::InputArray raw);
void oflow_run(struct oflow_context * context, cv::InputOutputArray raw, cv::Point2f& direction, float alpha);