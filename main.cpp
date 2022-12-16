#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include <opencv2/core/types_c.h>
#include <stdio.h>
using namespace cv;



void go_with_the_flow(Mat frame2, Mat next, Mat prvs, float alpha, Point2f& direction_fir)
{
    Mat flow(prvs.size(), CV_32FC2);
    calcOpticalFlowFarneback(prvs, next, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
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
            float angle = atan2(direction_fir.y, direction_fir.x);
            //snprintf(buf, 100, "%f %f    ", da_fir.x, da_fir.y);
            //snprintf(buf, 100, "%10.10f    ", sqrtf(da_fir.x*da_fir.x + da_fir.y*da_fir.y));
            snprintf(buf, 100, "%+5.0f    ", (angle / M_PI) * 180.0f);
            cv::putText(frame2, buf,c,cv::FONT_HERSHEY_DUPLEX,1,cv::Scalar(0,255,0),2,false);
            printf(buf);
        }
    }

}




void subimage(Mat raw, Mat next, Mat prvs, float alpha, Rect r[], Point2f directions[], int n)
{
    for (int i = 0; i < n; ++i)
    {
        go_with_the_flow(raw(r[i]), next(r[i]), prvs(r[i]), 0.1f, directions[i]);   
    }
    printf("\n");
}




int main(int argc, char const* argv[])
{
    setbuf(stdout, NULL);
    printf("Hello this is the Anteater! Opening file '%s' for videocapture!\n", argv[1]);
    if(argv[1] == NULL) {return 0;}

    VideoCapture capture(samples::findFile(argv[1]));
    if (!capture.isOpened())
    {
        printf("Unable to open file! %s\n", argv[1]);
        return 0;
    }

    
    int w = capture.get(CAP_PROP_FRAME_WIDTH);
    int h = capture.get(CAP_PROP_FRAME_HEIGHT);
    printf("Resolution %i %i\n", w, h);
#if 0
    // Four quadrants 
    #define NUM_OF_RECTS 4
    Rect r[NUM_OF_RECTS] = 
    {
        Rect (0  , 0  , w/2, h/2),
        Rect (w/2, 0  , w/2, h/2),
        Rect (0  , h/2, w/2, h/2),
        Rect (w/2, h/2, w/2, h/2),
    };
#endif
#if 1
    // Three quadrants across x axis
    #define NUM_OF_RECTS 3
    Rect r[NUM_OF_RECTS] = 
    {
        Rect ((0*w)/3, 0, w/3, h),
        Rect ((1*w)/3, 0, w/3, h),
        Rect ((2*w)/3, 0, w/3, h),
    };
#endif


    Point2f directions[3];
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

        subimage(raw, next, prvs, 0.1f, r, directions, NUM_OF_RECTS);
        imshow(argv[1], raw);

        prvs = next;
    }
    printf("Anteater exited successfully!\n");
    return 0;

capture_is_empty:
    printf("Unable to open file! %s\n", argv[1]);
    return 0;

}