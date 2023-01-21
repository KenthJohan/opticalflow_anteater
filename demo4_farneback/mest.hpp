#pragma once
#include <opencv2/core.hpp>




typedef struct
{
    cv::Mat flow;
    cv::Point2f dir;
    cv::Point2f dir_fir;
} motionest_state_t;

void motionest_init(motionest_state_t &state, cv::InputArray f1);
void motionest_progress(motionest_state_t &state, cv::InputArray f1, cv::InputArray f2);