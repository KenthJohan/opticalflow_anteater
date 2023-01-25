#include "mest_visual.hpp"
#include "draw.hpp"
#include "mest.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

void mest_visual_draw(cv::Mat frame, cv::Rect roi[], motionest_state_t mest[], int n)
{
    for(int i = 0; i < n; ++i)
    {
        draw_direction(frame(roi[i]), mest[i].dir_fir * 50.0f, cv::Scalar(200, 10, 50));
        cv::rectangle(frame, roi[i], cv::Scalar(0, 0, 255), 4);
    }

    cv::imshow("Frame", frame);
    draw_flow(mest[0].flow);
}