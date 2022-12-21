#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include "oflow.hpp"

using namespace cv;
using namespace std;

struct oflow_lucaskanade_context
{
    vector<Scalar> colors;
    RNG rng;
    Mat old_gray;
    vector<Point2f> p0, p1;
    Mat mask;
    int time;
};

void oflow_init(struct oflow_context * context, InputArray raw)
{
    context->internal = calloc(1, sizeof(oflow_lucaskanade_context));
    oflow_lucaskanade_context * internal = (oflow_lucaskanade_context *)context->internal;
    internal->rng.state = 1337;
    for(int i = 0; i < 100; i++)
    {
        int r = internal->rng.uniform(0, 256);
        int g = internal->rng.uniform(0, 256);
        int b = internal->rng.uniform(0, 256);
        //printf("%02X %02X %02X\n", r, g, b);
        internal->colors.push_back(Scalar(r,g,b));
    }
    // Take first frame and find corners in it
    cvtColor(raw, internal->old_gray, COLOR_BGR2GRAY);
    goodFeaturesToTrack(internal->old_gray, internal->p0, 100, 0.3, 7, Mat(), 7, false, 0.04);
    internal->mask = Mat::zeros(raw.size(), raw.type());
}


void oflow_run(struct oflow_context * context, InputOutputArray raw, Point2f& direction, float alpha)
{
    oflow_lucaskanade_context * internal = (oflow_lucaskanade_context *)context->internal;
    Mat frame_gray;
    cvtColor(raw, frame_gray, COLOR_BGR2GRAY);
    // calculate optical flow
    vector<uchar> status;
    vector<float> err;
    TermCriteria criteria = TermCriteria((TermCriteria::COUNT) + (TermCriteria::EPS), 10, 0.03);
    calcOpticalFlowPyrLK(internal->old_gray, frame_gray, internal->p0, internal->p1, status, err, Size(15,15), 2, criteria);
    vector<Point2f> good_new;
    Point2f d = {0,0};
    float n = 0;
    for(uint i = 0; i < internal->p0.size(); i++)
    {
        // Select good points
        if(status[i] == 1)
        {
            good_new.push_back(internal->p1[i]);
            // draw the tracks
            line(internal->mask, internal->p1[i], internal->p0[i], internal->colors[i], 1);
            circle(raw, internal->p1[i], 5, internal->colors[i], -1);
            d += (internal->p1[i] - internal->p0[i]);
            n += 1.0f;
        }
    }
    d /= n;
    direction.x = direction.x *(1.0f-alpha) + d.x * (alpha);
    direction.y = direction.y *(1.0f-alpha) + d.y * (alpha);
    copyTo(internal->mask, raw, internal->mask);

    internal->old_gray = frame_gray.clone();
    internal->p0 = good_new;


    internal->time++;
    if(internal->time >= 5)
    {
        internal->time = 0;
        //goodFeaturesToTrack(context->old_gray, context->p0, 100, 0.3, 7, Mat(), 7, false, 0.04);
    }
}