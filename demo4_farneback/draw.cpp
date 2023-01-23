#include "draw.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

void draw_crosshair(cv::Mat m)
{
    int x = m.cols/2;
    int y = m.cols/2;
    cv::line(m, cv::Point(x, 0), cv::Point(x, m.cols), cv::Scalar(100,100,0), 1);
    cv::line(m, cv::Point(0, y), cv::Point(m.rows, y), cv::Scalar(100,100,0), 1);
}

void draw_direction(cv::Mat m, cv::Point2f dir, const cv::Scalar& color)
{
    cv::Point2f o1 = cv::Point2f(m.cols / 2, m.rows / 2);
    cv::Point2f o2 = o1 + dir;
    cv::arrowedLine(m, o1, o2, color, 2);
}


void show_flow(cv::Mat flow)
{
    cv::Mat bgr; //Visual
    cv::Mat _hsv[3], hsv, hsv8;
    cv::Mat flow_parts[2];
    cv::Mat magnitude, angle, magn_norm;
    cv::split(flow, flow_parts);
    cv::cartToPolar(flow_parts[0], flow_parts[1], magnitude, angle, true);
    cv::normalize(magnitude, magn_norm, 0.0f, 1.0f, cv::NORM_MINMAX);
    angle *= ((1.f / 360.f) * (180.f / 255.f));
    _hsv[0] = angle;
    _hsv[1] = cv::Mat::ones(angle.size(), CV_32F);
    _hsv[2] = magn_norm;
    cv::merge(_hsv, 3, hsv);
    hsv.convertTo(hsv8, CV_8U, 255.0);
    cv::cvtColor(hsv8, bgr, cv::COLOR_HSV2BGR);
    cv::imshow("flow", bgr);
}

void draw_resize(cv::InputArray src, cv::OutputArray out, int scale)
{
    cv::resize(src, out, src.size()*scale, 0.1, 0.1, cv::INTER_NEAREST);
}


void draw_circle_center(cv::Mat m, float r)
{
    cv::Point center = cv::Point(m.cols / 2, m.rows / 2);
    cv::circle(m, center, r, cv::Scalar(10,10,200), 2);
}


void draw_text(cv::Mat img, const cv::String &text)
{
cv::putText(img,
            text,
            cv::Point(10, img.rows / 2), //top-left position
            cv::FONT_HERSHEY_DUPLEX,
            1.0,
            CV_RGB(118, 185, 0), //font color
            2);
}


void draw_text_float(cv::Mat img, cv::Point pos, cv::Scalar color, char const * fmt, float value)
{
    char buf[100];
    snprintf(buf, 100, fmt, value);
    cv::putText(img,
            buf,
           pos,
            cv::FONT_HERSHEY_DUPLEX,
            1.0,
            color,
            2);
}