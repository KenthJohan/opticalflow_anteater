#pragma once
#include <opencv2/core.hpp>

void draw_crosshair(cv::Mat m);
void show_flow(cv::Mat flow);
void draw_direction(cv::Mat m, cv::Point2f dir);
void draw_circle_center(cv::Mat m, float r);

void draw_resize(cv::InputArray src, cv::OutputArray out, int scale);
