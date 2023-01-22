#pragma once
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/video/background_segm.hpp>


typedef struct
{
    cv::Mat histogram;
    float flow_factor;
    cv::Ptr<cv::BackgroundSubtractor> pBackSub;
    cv::Mat fgMask;
    std::vector<cv::KeyPoint> keypoints;
    cv::Ptr<cv::SimpleBlobDetector> detector;
    cv::Point2f dir[3];
    
    cv::Point2f mean;
    cv::Point2f stddev;
    float ignore_radius;
} histvel_state_t;


void histvel_init(histvel_state_t &state);
void histvel_progress(histvel_state_t &state);
void histvel_add_flow(histvel_state_t &state, cv::Mat flow);
void histvel_draw(histvel_state_t &state);