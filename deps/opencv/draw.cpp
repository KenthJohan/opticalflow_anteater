#include "draw.h"
#include "common.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <assert.h>
#include "cvmats.hpp"




// For visual purpose only:
// This draw the direction and speed:
void draw_arrow(Tensor2_U8C3 * mat, Vec2i32 const * pos, Vec2i32 const * direction)
{
    cv::Mat m = mat2cvmat(mat);
    cv::Point2f c(pos->x, pos->y);
    cv::Point2f cd = c + cv::Point2f(direction->x, direction->y);
    arrowedLine(m, c, cd, cv::Scalar(255, 255, 255), 2, cv::LINE_4, 0, 0.5);
    //char buf[100];
    //float speed = HYPOT_F32(direction.x, direction.y);
    //snprintf(buf, 100, "%5.2f", speed);
    //snprintf(buf, 100, "%+5.0f    ", (angle / M_PI) * 180.0f);
    //cv::putText(frame, buf, cd, cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(0,255,0),2,false);
    //printf("%s\n", buf);
}




void draw_rectangle(Tensor2_U8C3 * mat, Vec2i32 const * pos, Vec2i32 const * length)
{
    cv::Mat m = mat2cvmat(mat);
    cv::rectangle(m, cv::Rect(pos->x, pos->y, length->x, length->y), cv::Scalar(0, 0, 255), 2, cv::LINE_4);
}

void draw_show(char const * title, Tensor2_U8C3 * mat)
{
    cv::Mat m = mat2cvmat(mat);
    cv::imshow(title, m);
    {
        int keyboard = cv::waitKey(30);
        if (keyboard == 'q' || keyboard == 27) {}
    }
}


/*
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
*/

void draw_weighed(Tensor2_U8C3 * mat1, double alpha, Tensor2_U8C3 * mat2, double beta, double gamma, Tensor2_U8C3 * dst)
{
    cv::Mat s1 = mat2cvmat(mat1);
    cv::Mat s2 = mat2cvmat(mat2);
    cv::Mat d = mat2cvmat(dst);
    assert(s1.data == mat1->start);
    assert(s2.data == mat2->start);
    assert(d.data == dst->start);
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
    return;
}