#include "histvel.hpp"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include <opencv2/features2d.hpp>

#include "draw.hpp"

using namespace cv;
using namespace std;


#define HVEL_GAIN 10.0f


void histo_store(Point2f p, Mat out)
{
    Point q = Point((int)p.x, (int)p.y);
    if(q.x >= out.cols) {return;}
    if(q.y >= out.rows) {return;}
    if(q.x < 0) {return;}
    if(q.y < 0) {return;}
    out.at<uint8_t>(q) += 1;
}


void histo_cartisan(Mat m, Mat out)
{
    for (MatIterator_<Vec2f> it = m.begin<Vec2f>(); it != m.end<Vec2f>(); ++it)
    {
        float x = (*it)[0];
        float y = (*it)[1];
        if (x == 0){continue;}
        if (y == 0){continue;}
        if (isnanf(x)){continue;}
        if (isnanf(y)){continue;}
        if (isinf(x)){continue;}
        if (isinf(y)){continue;}
        x *= HVEL_GAIN;
        y *= HVEL_GAIN;
        x += out.cols/2;
        y += out.rows/2;
        histo_store(Point2f(x, y), out);
    }
}

void histo_polar(Mat m, Mat out)
{
    for (MatIterator_<Vec2f> it = m.begin<Vec2f>(); it != m.end<Vec2f>(); ++it)
    {
        float x = (*it)[0];
        float y = (*it)[1];
        if (x == 0){continue;}
        if (y == 0){continue;}
        if (isnanf(x)){continue;}
        if (isnanf(y)){continue;}
        if (isinf(x)){continue;}
        if (isinf(y)){continue;}
        float angle = atan2f(y, x);
        float radius = sqrtf(x*x + y*y);
        angle += M_PI;
        x = (angle) / (2.0f*M_PI);
        x *= out.cols;
        y = radius + (out.rows / 2);
        histo_store(Point2f(x, y), out);
    }
}





void histvel_init(histvel_state_t &state)
{
    state.histogram = Mat(Size(200, 200), CV_8U);
    state.histogram.setTo(0);
    state.pBackSub = createBackgroundSubtractorMOG2(3, 10);
    SimpleBlobDetector::Params params;
    // Change thresholds
    params.minThreshold = 50;
    params.maxThreshold = 255;
    // Filter by Area.
    params.filterByArea = true;
    params.minArea = 5;
    params.minArea = 20;
    // Filter by Circularity
    params.filterByCircularity = false;
    params.minCircularity = 0.1;
    // Filter by Convexity
    params.filterByConvexity = false;
    params.minConvexity = 0.3;
    // Filter by Inertia
    params.filterByInertia = false;
    params.minInertiaRatio = 0.01;
    state.detector = SimpleBlobDetector::create(params);
}







void histvel_add_flow(histvel_state_t &state, Mat flow)
{
    {
        Scalar scalar_mean;
        Scalar scalar_stddev;
        meanStdDev(flow, scalar_mean, scalar_stddev);
        Point2f mean = Point2f(scalar_mean[0], scalar_mean[1]);
        Point2f stddev = Point2f(scalar_stddev[0], scalar_stddev[1]);
        float alpha = 0.5f;
        state.mean = (state.mean * (1.0f - alpha)) + (mean * alpha);
        state.stddev = (state.stddev * (1.0f - alpha)) + (stddev * alpha);
    }


    //printf("%f %f\n", state.stddev[0], state.stddev[1]);
    histo_cartisan(flow, state.histogram);
    //histo_polar(flow, state.histogram);

    //GaussianBlur(state.histogram, state.histogram, Size(3, 3), 0);

}




void histvel_progress(histvel_state_t &state)
{
    //state.detector->detect(state.histogram, state.keypoints);
    Point c = Point(state.histogram.cols / 2, state.histogram.rows / 2);

    {
        state.ignore_radius = state.stddev.x + state.stddev.y;
        state.ignore_radius *= 10.0f;
    }

    {
        double min_val;
        double max_val;
        Point min_point;
        Point max_point;
        Mat h = state.histogram.clone();
        circle(h, c, state.ignore_radius, Scalar(0), -1);
        minMaxLoc(h, &min_val, &max_val, &min_point, &max_point);
        max_point -= c;
        Point2f dir = max_point;
        dir /= HVEL_GAIN;
        float alpha = 0.7f;
        state.dir = (state.dir * (1.0f - alpha)) + (dir * alpha);
    }
}


#define UPSCALE 6

void histvel_draw(histvel_state_t &state)
{
    //printf("%i\n", state.keypoints.size());
    //Mat im;
    //drawKeypoints(state.histogram, state.keypoints, im, Scalar(0, 0, 100), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    //state.pBackSub->apply(state.h, state.fgMask);
    //Mat bgMask;
    //state.pBackSub->getBackgroundImage(bgMask);
    Mat bgr;
    cvtColor(state.histogram, bgr, COLOR_GRAY2BGR);

    //printf("%i %i %f\n", max_point.x, max_point.y, max_val);
    //cv::arrowedLine(bgr, cv::Point(bgr.cols / 2, bgr.rows / 2), max_point, cv::Scalar(255, 0, 0), 1);
    
    Mat big;
    draw_resize(bgr, big, UPSCALE);
    draw_crosshair(big);
    draw_direction(big, state.dir*UPSCALE*HVEL_GAIN);
    draw_circle_center(big, state.ignore_radius*UPSCALE);
    cv::imshow("VelocitiesHistgram", big);
}






