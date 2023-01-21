#pragma once
#include <opencv2/core.hpp>

void draw_crosshair(cv::Mat m);
void show_flow(cv::Mat flow);
void draw_direction(cv::Mat m, cv::Point2f dir);