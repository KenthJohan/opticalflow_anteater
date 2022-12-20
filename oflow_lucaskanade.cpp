#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include "oflow_lucaskanade.hpp"

using namespace cv;
using namespace std;



void oflow_lucaskanade_init(struct oflow_lucaskanade_context * context, InputArray raw)
{
    for(int i = 0; i < 100; i++)
    {
        int r = context->rng.uniform(0, 256);
        int g = context->rng.uniform(0, 256);
        int b = context->rng.uniform(0, 256);
        context->colors.push_back(Scalar(r,g,b));
    }


    // Take first frame and find corners in it
    cvtColor(raw, context->old_gray, COLOR_BGR2GRAY);
    goodFeaturesToTrack(context->old_gray, context->p0, 100, 0.3, 7, Mat(), 7, false, 0.04);
    context->mask = Mat::zeros(raw.size(), raw.type());

}


void oflow_lucaskanade_run(struct oflow_lucaskanade_context * context, InputOutputArray raw, Point2f& direction)
{
    Mat frame_gray;
    cvtColor(raw, frame_gray, COLOR_BGR2GRAY);
    // calculate optical flow
    vector<uchar> status;
    vector<float> err;
    TermCriteria criteria = TermCriteria((TermCriteria::COUNT) + (TermCriteria::EPS), 10, 0.03);
    calcOpticalFlowPyrLK(context->old_gray, frame_gray, context->p0, context->p1, status, err, Size(15,15), 2, criteria);
    vector<Point2f> good_new;
    for(uint i = 0; i < context->p0.size(); i++)
    {
        // Select good points
        if(status[i] == 1)
        {
            good_new.push_back(context->p1[i]);
            // draw the tracks
            line(context->mask, context->p1[i], context->p0[i], context->colors[i], 1);
            circle(raw, context->p1[i], 5, context->colors[i], -1);
            Point2d d = context->p1[i] - context->p0[i];
            float alpha = 0.01f;
            direction.x = direction.x *(1.0f-alpha) + d.x * (alpha);
            direction.y = direction.y *(1.0f-alpha) + d.y * (alpha);
        }
    }
    copyTo(context->mask, raw, context->mask);

    context->old_gray = frame_gray.clone();
    context->p0 = good_new;
}