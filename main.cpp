#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include <opencv2/core/utils/filesystem.hpp>
#include <opencv2/core/types_c.h>
#include <stdio.h>
using namespace cv;

#define HYPOT_F64(x,y) (sqrt((x)*(x) + (y)*(y)))
#define HYPOT_F32(x,y) (sqrtf((x)*(x) + (y)*(y)))

void go_with_the_flow(Mat frame2, Mat next, Mat prvs, float alpha, Point2f& direction_fir)
{
    Mat flow(prvs.size(), CV_32FC2);
    double pyr_scale = 0.5;
    int levels = 3;
    int winsize = 5;
    int iterations = 3;
    int poly_n = 7;
    double poly_sigma = 1.2;
    int flags = 0;
    calcOpticalFlowFarneback(prvs, next, flow, pyr_scale, levels, winsize, iterations, poly_n, poly_sigma, flags);
    // visualization
    Mat flowp[2];
    split(flow, flowp);
    Mat magnitude, angle, magn_norm;
    cartToPolar(flowp[0], flowp[1], magnitude, angle, true);
    normalize(magnitude, magn_norm, 0.0f, 1.0f, NORM_MINMAX);
    angle *= ((1.f / 360.f) * (180.f / 255.f));
    //build hsv image
    Mat _hsv[3], hsv, hsv8, bgr;
    _hsv[0] = angle;
    _hsv[1] = Mat::ones(angle.size(), CV_32F);
    _hsv[2] = magn_norm;
    merge(_hsv, 3, hsv);
    hsv.convertTo(hsv8, CV_8U, 255.0);
    cvtColor(hsv8, bgr, COLOR_HSV2BGR);
    
    
    
    //Mat overlay;
    addWeighted(bgr, 0.5, frame2, 0.5, 0.0, frame2);
    {
        // Direction average calculation:
        Point2f direction = Point2f(0,0);
        for(int j = 0;j < flow.cols;j++){
            for(int i = 0;i < flow.rows;i++){
                direction.x += flowp[0].at<float>(i,j);
                direction.y += flowp[1].at<float>(i,j);
            }
        }
        direction.x *= (1.0f/(flow.cols*flow.rows));
        direction.y *= (1.0f/(flow.cols*flow.rows));
        
        // Direction average FIR calculation, FIR filter N=1
        direction_fir.x = direction_fir.x *(1.0f-alpha) + direction.x * (alpha);
        direction_fir.y = direction_fir.y *(1.0f-alpha) + direction.y * (alpha);

        {
            // Draw line from (c) to (cd):
            Point2f c = Point2f((float)frame2.cols / 2.0f, (float)frame2.rows / 2.0f);
            Point2f cd = c + direction_fir*-60.0f;
            arrowedLine(frame2, c, cd, Scalar(255, 255, 255), 2, LINE_4, 0, 0.5);
            char buf[100];

            // Print angle:
            //float angle = atan2(direction_fir.y, direction_fir.x);
            direction_fir;
            float speed = HYPOT_F32(direction_fir.x, direction_fir.y);
            //snprintf(buf, 100, "%f %f    ", da_fir.x, da_fir.y);
            snprintf(buf, 100, "%5.2f", speed);
            //snprintf(buf, 100, "%+5.0f    ", (angle / M_PI) * 180.0f);
            cv::putText(frame2, buf, cd, cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(0,255,0),2,false);
            printf(buf);
        }
    }

}




void subimage(Mat raw, Mat next, Mat prvs, float alpha[], Rect r[], Point2f directions[], int n)
{
    for (int i = 0; i < n; ++i)
    {
        go_with_the_flow(raw(r[i]), next(r[i]), prvs(r[i]), alpha[i], directions[i]);
        rectangle(raw, r[i], Scalar(255, 255, 255), 6, LINE_4);
    }
    printf("\n");
}



void rect_init_three_way(Rect r[3], int w, int h)
{
    r[0] = Rect ((0*w)/3, 0, w/3, h);
    r[1] = Rect ((1*w)/3, 0, w/3, h);
    r[2] = Rect ((2*w)/3, 0, w/3, h);
}
void rect_init_four_way(Rect r[4], int w, int h)
{
    r[0] = Rect (0  , 0  , w/2, h/2);
    r[1] = Rect (w/2, 0  , w/2, h/2);
    r[2] = Rect (0  , h/2, w/2, h/2);
    r[3] = Rect (w/2, h/2, w/2, h/2);
}

int main(int argc, char const* argv[])
{
    VideoCapture capture;

    setbuf(stdout, NULL);
    printf("Hello this is the Anteater!\n");
    printf("cwd: '%s'\n", cv::utils::fs::getcwd().c_str());
    if(argv[1] == NULL)
    {
        printf("Missing input file!\n");
        return 0;
    }

    
    {
        cv::String path = samples::findFile(argv[1], false, true);
        if(path.empty())
        {
            printf("Cannot find file '%s'!\n", argv[1]);
            return 0;
        }
        printf("Opening file '%s'  for videocapture!\n", path.c_str());
        capture.open(path);
        if (!capture.isOpened())
        {
            printf("Unable to open file! %s\n", path.c_str());
            return 0;
        }
    }


    
    int w = capture.get(CAP_PROP_FRAME_WIDTH);
    int h = capture.get(CAP_PROP_FRAME_HEIGHT);
    printf("Resolution of videocapture is %ix%i!\n", w, h);

    /*
    #define NUM_OF_VIEWS 4
    Rect r[NUM_OF_VIEWS];
    rect_init_four_way(r, w, h);
    */

    #define NUM_OF_VIEWS 3
    Rect r[NUM_OF_VIEWS];
    float alpha[NUM_OF_VIEWS] = {0.1f, 0.1f, 0.1f};
    rect_init_three_way(r, w, h);


    Point2f directions[NUM_OF_VIEWS];
    Mat raw;
    Mat prvs;
    capture >> raw;
    if (raw.empty()){goto capture_is_empty;}
    cvtColor(raw, prvs, COLOR_BGR2GRAY);

    while(true)
    {
        int keyboard = waitKey(30);
        if (keyboard == 'q' || keyboard == 27) {break;}
        Mat next;
        capture >> raw;
        if (raw.empty()){goto capture_is_empty;}
        cvtColor(raw, next, COLOR_BGR2GRAY);

        subimage(raw, next, prvs, alpha, r, directions, NUM_OF_VIEWS);
        imshow(argv[1], raw);

        prvs = next;
    }
    printf("Anteater exited successfully!\n");
    return 0;

capture_is_empty:
    printf("Capture is empty! %s\n", argv[1]);
    return 0;

}