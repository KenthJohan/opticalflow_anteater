#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include <opencv2/features2d.hpp>


#include "mest.hpp"
#include "draw.hpp"

using namespace cv;
using namespace std;






Point2f get_direction(Mat m)
{
    float dx = 0;
    float dy = 0;
    for (MatIterator_<Vec2f> it = m.begin<Vec2f>(); it != m.end<Vec2f>(); ++it)
    {
        float dx0 = (*it)[0];
        float dy0 = (*it)[1];
        dx += dx0;
        dy += dy0;
    }
    return Point2f(dx, dy);
}



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











void motionest_init(motionest_state_t &state, cv::InputArray f1)
{
    state.h = Mat(Size(200, 200), CV_8U);
    state.h.setTo(0);
    state.flow = Mat(f1.size(), CV_32FC2);
    state.dir_fir = Point2f(0,0);
    int n = state.flow.rows * state.flow.cols;
    state.flow_factor = 1.0f / ((float)n);
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







void motionest_progress(motionest_state_t &state, InputArray f1, InputArray f2)
{
    state.flow.setTo(Scalar(0.0, 0.0));
    calcOpticalFlowFarneback(f1, f2, state.flow, 0.5, 3, 15, 3, 5, 1.2, 0);
    

    /*
    {
        double vmax;
        double vmin;
        Point imax;
        Point imin;
        minMaxLoc(state.h, &vmin, &vmax, &imin, &imax);
        imax.x -= state.h.cols / 2;
        imax.y -= state.h.rows / 2;
        printf("%f, %i, %i\n", vmax, imax.x, imax.y);
    }
    */

// Set up the detector with default parameters.

 
// Detect blobs.

    


    /**/
    state.dir = get_direction(state.flow);
    state.dir *= state.flow_factor;
    float alpha = 0.1f;
    state.dir_fir = state.dir_fir * (1.0f - alpha) + (state.dir * alpha);

    
    state.h.setTo(0);
    histo_cartisan(state.flow, state.h);
    //GaussianBlur(state.h, state.h, Size(3, 3), 0);
    state.detector->detect(state.h, state.keypoints);
    //histo_polar(state.flow, state.h);
    {
        printf("%i\n", state.keypoints.size());
        Mat im;
        drawKeypoints(state.h, state.keypoints, im, Scalar(0, 0, 100), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        //state.pBackSub->apply(state.h, state.fgMask);
        //Mat bgMask;
        //state.pBackSub->getBackgroundImage(bgMask);
        Mat m;
        resize(im, m, im.size()*6, 0.1, 0.1, INTER_NEAREST);
        draw_crosshair(m);
        imshow("Hist", m);
    }

}



