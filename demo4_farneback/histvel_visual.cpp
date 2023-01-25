#include "histvel_visual.hpp"
#include "histvel.hpp"
#include "draw.hpp"
#include <opencv2/plot.hpp>




void histvel_visual_init(histvel_visual_t &state)
{

}















#define UPSCALE 7


#define EUCLIDEAN_NORM(x,y) sqrtf(x*x + y*y)

// Temorary stuff:
#define MATERIAL_LENGTH 0.25
#define WELDING_DURATION 12.0
#define WELD_SPEED (MATERIAL_LENGTH/WELDING_DURATION)
#define WELD_PIX_AVG_SPEED 2.7
#define THE_FACTOR (WELD_SPEED / WELD_PIX_AVG_SPEED)


void histvel_visual_draw(histvel_visual_t &visual, histvel_state_t &state)
{
    //printf("%i\n", state.keypoints.size());
    //Mat im;
    //drawKeypoints(state.histogram, state.keypoints, im, Scalar(0, 0, 100), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    //state.pBackSub->apply(state.h, state.fgMask);
    //Mat bgMask;
    //state.pBackSub->getBackgroundImage(bgMask);
    state.histogram *= 2;
    cv::Mat bgr;
    cvtColor(state.histogram, bgr, cv::COLOR_GRAY2BGR);

    //printf("%i %i %f\n", max_point.x, max_point.y, max_val);
    //cv::arrowedLine(bgr, cv::Point(bgr.cols / 2, bgr.rows / 2), max_point, cv::Scalar(255, 0, 0), 1);
    
    cv::Mat big;
    draw_resize(bgr, big, UPSCALE);
    draw_crosshair(big);
    draw_direction(big, state.dir[0]*UPSCALE*HVEL_GAIN, CV_RGB(255, 0, 0));
    //draw_direction(big, state.dir[1]*UPSCALE*HVEL_GAIN, CV_RGB(0, 255, 0));
    //draw_direction(big, state.dir[2]*UPSCALE*HVEL_GAIN, CV_RGB(0, 0, 255));

    


    draw_circle_center(big, state.ignore_radius*UPSCALE);
    float mag1 = EUCLIDEAN_NORM(state.dir[0].x, state.dir[0].y);
    float mag2 = EUCLIDEAN_NORM(state.dir[1].x, state.dir[1].y);
    float mag3 = EUCLIDEAN_NORM(state.dir[2].x, state.dir[2].y);
    
    state.vels.push_back(mag3);

    state.speed_sum += mag1;
    state.speed_n += 1;
    draw_text_float(big, cv::Point(big.cols - 300, 50+10), CV_RGB(255, 255, 255), "ref %7.4f m/s", WELD_SPEED);
    draw_text_float(big, cv::Point(big.cols - 300, 50+40), CV_RGB(255, 0, 0), " f0 %7.4f m/s", mag1*THE_FACTOR);
    draw_text_float(big, cv::Point(big.cols - 300, 50+70), CV_RGB(0, 255, 0), " f1 %7.4f m/s", mag2*THE_FACTOR);
    draw_text_float(big, cv::Point(big.cols - 300, 50+100), CV_RGB(0, 0, 255), " f2 %7.4f m/s", mag3*THE_FACTOR);
    cv::imshow("VelocitiesHistgram", big);

    visual.plot = cv::plot::Plot2d::create(state.vels);
    visual.plot->setInvertOrientation(true);
    visual.plot->setShowText(false);
    cv::Mat display;
    visual.plot->render(display);
    cv::imshow("Graph Plot", display);

}