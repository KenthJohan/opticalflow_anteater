#include "draw.h"
#include "common.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <assert.h>

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

void draw_show(char const * title, Mat * mat)
{
    cv::Mat m = cv::Mat(mat->shape[0], mat->shape[1], mat->type, mat->start);
    cv::imshow(title, m);
    {
        int keyboard = cv::waitKey(30);
        if (keyboard == 'q' || keyboard == 27) {}
    }
}


void draw_weighed(Mat * mat1, double alpha, Mat * mat2, double beta, double gamma, Mat * dst)
{
    cv::Rect r1 = cv::Rect(0, 0, mat1->shape[0], mat1->shape[1]);
    cv::Rect r2 = cv::Rect(100, 100, mat1->shape[0], mat1->shape[1]);
    cv::Mat s1 = cv::Mat(mat1->shape[0], mat1->shape[1], mat1->type, mat1->start);
    cv::Mat s2 = cv::Mat(mat2->shape[0], mat2->shape[1], mat2->type, mat2->start)(r1);
    cv::Mat d = cv::Mat(dst->shape[0], dst->shape[1], dst->type, dst->start)(r2);
    assert(s1.rows == s2.rows);
    assert(s1.cols == s2.cols);
    assert(s1.type() == s2.type());
    assert(s1.rows == d.rows);
    assert(s1.cols == d.cols);
    assert(s1.type() == d.type());
    //d += s1;
    cv::addWeighted(s1, alpha, s2, beta, gamma, d);
}