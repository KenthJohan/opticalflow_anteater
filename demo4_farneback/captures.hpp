#pragma once

#include <opencv2/videoio.hpp>

typedef struct
{
    cv::VideoCapture capture;
    cv::Mat frame;
    cv::Mat f1;
    cv::Mat f2;
    int frame_index = 0;
} captures_t;

void captures_reset(captures_t &state);
void captures_init(captures_t &state, std::string filename);
void captures_progress(captures_t &state);
void captures_reset(captures_t &state);