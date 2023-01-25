#pragma once

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/plot.hpp>

#include "histvel.hpp"

typedef struct
{
    std::vector<double> vels;
	cv::Ptr<cv::plot::Plot2d> plot;
    float speed_sum;
    float speed_n;
} histvel_visual_t;


void histvel_visual_init(histvel_visual_t &state);
void histvel_visual_reset(histvel_visual_t &visual);
void histvel_visual_draw(histvel_visual_t &visual, histvel_state_t &state);