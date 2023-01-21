#pragma once
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include <opencv2/features2d.hpp>

typedef struct
{
    cv::Mat flow;
    cv::Point2f dir;
    cv::Point2f dir_fir;
    float flow_factor;
    cv::Mat h;

    cv::Ptr<cv::BackgroundSubtractor> pBackSub;
    cv::Mat fgMask;
    std::vector<cv::KeyPoint> keypoints;
    cv::Ptr<cv::SimpleBlobDetector> detector;
} motionest_state_t;

void motionest_init(motionest_state_t &state, cv::InputArray f1);
void motionest_progress(motionest_state_t &state, cv::InputArray f1, cv::InputArray f2);