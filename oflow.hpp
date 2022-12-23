#pragma once
#include <opencv2/core/types_c.h>

struct oflow_context
{
    void * internal;
};

void oflow_init(struct oflow_context * context, void * raw, int type, int rows, int cols);
void oflow_run(struct oflow_context * context, void * raw, int type, int rows, int cols, float direction[2], float alpha, int cropx, int cropy, int cropw, int croph);