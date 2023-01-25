#pragma once
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/video/background_segm.hpp>



#define HVEL_GAIN 10.0f

typedef struct
{
    cv::Mat histogram;

    cv::Point2f dir[3];
    
    cv::Point2f mean;
    cv::Point2f stddev;
    float ignore_radius;

    float speed_sum;
    float speed_n;

    std::vector<double> vels;

} histvel_state_t;



void histvel_state_init(histvel_state_t &state);
void histvel_state_progress(histvel_state_t &state);
void histvel_state_add_flow(histvel_state_t &state, cv::Mat flow);
