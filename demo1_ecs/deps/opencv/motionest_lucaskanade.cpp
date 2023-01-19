#include "motionest_lucaskanade.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>

#include "cvmats.hpp"

struct oflow_lucaskanade_context
{
    std::vector<cv::Scalar> colors;
    cv::RNG rng;
    cv::Mat old_gray;
    std::vector<cv::Point2f> p0, p1;
    cv::Mat mask;
    int time;
};

void motionest_lucaskanade_init(struct oflow_context * context, Tensor2_U8C3 * raw)
{
    //cv::Mat cvraw = mat2cvmat(raw);
    context->internal = calloc(1, sizeof(oflow_lucaskanade_context));
    oflow_lucaskanade_context * internal = (oflow_lucaskanade_context *)context->internal;
    internal->rng.state = 1337;
    for(int i = 0; i < 100; i++)
    {
        int r = internal->rng.uniform(0, 256);
        int g = internal->rng.uniform(0, 256);
        int b = internal->rng.uniform(0, 256);
        //printf("%02X %02X %02X\n", r, g, b);
        internal->colors.push_back(cv::Scalar(r,g,b));
    }
    // Take first frame and find corners in it
    /*
    cv::cvtColor(cvraw, internal->old_gray, cv::COLOR_BGR2GRAY);
    cv::goodFeaturesToTrack(internal->old_gray, internal->p0, 100, 0.3, 7, cv::Mat(), 7, false, 0.04);
    internal->mask = cv::Mat::zeros(cvraw.size(), cvraw.type());
    */
}


void motionest_lucaskanade_run(struct oflow_context * context, Tensor2_U8C3 * raw, Vec2f32 * vel, float alpha)
{
    cv::Mat cvraw = mat2cvmat(raw);
    oflow_lucaskanade_context * internal = (oflow_lucaskanade_context *)context->internal;
    cv::Mat frame_gray;
    cv::cvtColor(cvraw, frame_gray, cv::COLOR_BGR2GRAY);
    // calculate optical flow
    std::vector<uchar> status;
    std::vector<float> err;
    cv::TermCriteria criteria = cv::TermCriteria((cv::TermCriteria::COUNT) + (cv::TermCriteria::EPS), 10, 0.03);
    cv::calcOpticalFlowPyrLK(internal->old_gray, frame_gray, internal->p0, internal->p1, status, err, cv::Size(15,15), 2, criteria);
    std::vector<cv::Point2f> good_new;
    cv::Point2f d = {0,0};
    float n = 0;
    for(uint i = 0; i < internal->p0.size(); i++)
    {
        // Select good points
        if(status[i] == 1)
        {
            good_new.push_back(internal->p1[i]);
            // draw the tracks
            cv::line(internal->mask, internal->p1[i], internal->p0[i], internal->colors[i], 1);
            cv::circle(cvraw, internal->p1[i], 5, internal->colors[i], -1);
            d += (internal->p1[i] - internal->p0[i]);
            n += 1.0f;
        }
    }
    d /= n;
    vel->x = vel->x *(1.0f-alpha) + d.x * (alpha);
    vel->y = vel->y *(1.0f-alpha) + d.y * (alpha);
    cv::copyTo(internal->mask, cvraw, internal->mask);

    internal->old_gray = frame_gray.clone();
    internal->p0 = good_new;

    internal->time++;
    if(internal->time >= 5)
    {
        internal->time = 0;
        cv::goodFeaturesToTrack(internal->old_gray, internal->p0, 100, 0.3, 7, cv::Mat(), 7, false, 0.04);
    }
}