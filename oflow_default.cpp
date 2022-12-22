#include "oflow.hpp"
#include <opencv2/core/types_c.h>
using namespace cv;


struct oflow_default_context
{
    int dummy;
};


void oflow_init(struct oflow_context * context, InputArray raw)
{
    context->internal = calloc(1, sizeof(oflow_default_context));
    oflow_default_context * internal = (oflow_default_context *)context->internal;
}


void oflow_run(struct oflow_context * context, InputOutputArray raw, Point2f& direction, float alpha)
{
    oflow_default_context * internal = (oflow_default_context *)context->internal;
}