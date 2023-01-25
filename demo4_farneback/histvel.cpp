#include "histvel.hpp"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/plot.hpp>



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





void histvel_state_init(histvel_state_t &state)
{
    state.histogram = Mat(Size(150, 150), CV_8U);
    state.histogram.setTo(0);
}

void histvel_state_reset(histvel_state_t &state)
{
    state.histogram.setTo(0);
}






void histvel_state_add_flow(histvel_state_t &state, Mat flow)
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




#define EUCLIDEAN_NORM(x,y) sqrtf(x*x + y*y)

void histvel_state_progress(histvel_state_t &state)
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
        float alpha1 = 1.0f;
        float alpha2 = 0.5f;
        float alpha3 = 0.1f;
        state.dir[0] = (state.dir[0] * (1.0f - alpha1)) + (dir * alpha1);
        state.dir[1] = (state.dir[1] * (1.0f - alpha2)) + (dir * alpha2);
        state.dir[2] = (state.dir[2] * (1.0f - alpha3)) + (dir * alpha3);
    }

    {
        state.mag0 = EUCLIDEAN_NORM(state.dir[0].x, state.dir[0].y);
        state.mag1 = EUCLIDEAN_NORM(state.dir[1].x, state.dir[1].y);
        state.mag2 = EUCLIDEAN_NORM(state.dir[2].x, state.dir[2].y);
    }

    
}









