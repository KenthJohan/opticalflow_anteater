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



void draw_weighed1(int32_t type, int32_t shape[], void * mat1, int32_t steps1[], double alpha, void * mat2, int32_t steps2[], double beta, double gamma, void * mat3, int32_t steps3[])
{
    size_t ssteps1[] = {steps1[0],steps1[1]};
    size_t ssteps2[] = {steps2[0],steps2[1]};
    size_t ssteps3[] = {steps3[0],steps3[1]};
    cv::Mat m1 = cv::Mat(2, shape, type, mat1, ssteps1);
    cv::Mat m2 = cv::Mat(2, shape, type, mat2, ssteps2);
    cv::Mat m3 = cv::Mat(2, shape, type, mat3, ssteps3);
    cv::addWeighted(m1, alpha, m2, beta, gamma, m3);
}

void draw_weighed(Mat * mat1, double alpha, Mat * mat2, double beta, double gamma, Mat * dst)
{
    //cv::Rect r1 = cv::Rect(0, 0, mat1->shape[0], mat1->shape[1]);
    //cv::Rect r2 = cv::Rect(100, 100, mat1->shape[0], mat1->shape[1]);
    int sizes1[] = {mat1->shape[0],mat1->shape[1]};
    int sizes2[] = {mat2->shape[0],mat2->shape[1]};
    int sizes3[] = {mat1->shape[0],mat1->shape[1]};
    size_t steps1[] = {mat1->step[0],mat1->step[1]};
    size_t steps2[] = {mat2->step[0],mat2->step[1]};
    size_t steps3[] = {dst->step[0],dst->step[1]};
    cv::Mat s1 = cv::Mat(mat1->dims, sizes1, mat1->type, mat1->start, steps1);
    cv::Mat s2 = cv::Mat(mat2->dims, sizes2, mat2->type, mat2->start, steps2);
    cv::Mat d = cv::Mat(dst->dims, sizes3, dst->type, dst->start, steps3);
    assert(s1.dims == 2);
    assert(s2.dims == 2);
    assert(d.dims == 2);
    assert(s1.rows == s2.rows);
    assert(s1.cols == s2.cols);
    assert(s1.type() == s2.type());
    assert(s1.rows == d.rows);
    assert(s1.cols == d.cols);
    assert(s1.type() == d.type());
    //d += s1;
    cv::addWeighted(s1, alpha, s2, beta, gamma, d);
}