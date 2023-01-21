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





void histo_store(Point2f p, Mat out)
{
    Point q = Point((int)p.x, (int)p.y);
    if(q.x >= out.cols) {return;}
    if(q.y >= out.rows) {return;}
    if(q.x < 0) {return;}
    if(q.y < 0) {return;}
    out.at<uint8_t>(q) += 10;
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
        x *= 10.0f;
        y *= 10.0f;
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












void histvel_progress(histvel_state_t &state, Mat flow)
{
    histo_cartisan(flow, state.histogram);
    //GaussianBlur(state.h, state.h, Size(3, 3), 0);
    state.detector->detect(state.histogram, state.keypoints);
    //histo_polar(state.flow, state.h);
    {
        printf("%i\n", state.keypoints.size());
        Mat im;
        drawKeypoints(state.histogram, state.keypoints, im, Scalar(0, 0, 100), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        //state.pBackSub->apply(state.h, state.fgMask);
        //Mat bgMask;
        //state.pBackSub->getBackgroundImage(bgMask);
        Mat m;
        resize(im, m, im.size()*6, 0.1, 0.1, INTER_NEAREST);
        draw_crosshair(m);
        imshow("Hist", m);
    }

}










