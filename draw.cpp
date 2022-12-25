#include "draw.h"
#include "common.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

// For visual purpose only:
// This draw the direction and speed:
void draw_arrow(void * image, int type, Vec2i32 resolution, Vec2i32 rio_pos, Vec2i32 rio_size, Vec2f32 direction, float gain)
{
    cv::Rect cvrio = cv::Rect(rio_pos.x, rio_pos.y, rio_size.x, rio_size.y);
    cv::Mat frame = cv::Mat(resolution.y, resolution.x, type, image)(cvrio);
    // Draw line from Point(c) to Point(c+d):
    // Where Point(c) is the center of frame:
    cv::Point2f c = cv::Point2f((float)frame.cols / 2.0f, (float)frame.rows / 2.0f);
    cv::Point2f cd = c + cv::Point2f(direction.x*gain, direction.y*gain);
    arrowedLine(frame, c, cd, cv::Scalar(255, 255, 255), 2, cv::LINE_4, 0, 0.5);
    char buf[100];
    float speed = HYPOT_F32(direction.x, direction.y);
    snprintf(buf, 100, "%5.2f", speed);
    //snprintf(buf, 100, "%+5.0f    ", (angle / M_PI) * 180.0f);
    cv::putText(frame, buf, cd, cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(0,255,0),2,false);
    //printf("%s\n", buf);
}


void draw_rectangle(void * image, int type, Vec2i32 resolution, Vec2i32 pos, Vec2i32 length)
{
    cv::Mat mat = cv::Mat(resolution.y, resolution.x, type, image);
    cv::rectangle(mat, cv::Rect(pos.x, pos.y, length.x, length.y), cv::Scalar(0, 0, 255), 2, cv::LINE_4);
}

void draw_show(void * image, int type, Vec2i32 resolution)
{
    cv::Mat mat = cv::Mat(resolution.y, resolution.x, type, image);
    cv::imshow("hej!", mat);
}