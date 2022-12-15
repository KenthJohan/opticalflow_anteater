#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include <opencv2/core/types_c.h>
#include <stdio.h>
using namespace cv;



void go_with_the_flow(Mat frame2, Mat next, Mat prvs)
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
    
    
    
    Mat overlay;
    addWeighted(bgr, 0.5, frame2, 0.5, 0.0, overlay);
    {
        Point2f da = Point2f(0,0);
        for(int j = 0;j < flow.cols;j++){
            for(int i = 0;i < flow.rows;i++){
                da.x += flowp[0].at<float>(i,j);
                da.y += flowp[1].at<float>(i,j);
            }
        }
        da.x *= (1.0f/(flow.cols*flow.rows));
        da.y *= (1.0f/(flow.cols*flow.rows));
        static Point2f d = Point2f(0,0);
        for(int j = 0;j < flow.cols;j++){
            for(int i = 0;i < flow.rows;i++){
                float a = 0.0001f;
                d.x = d.x *(1.0f-a) + da.x * (a);// FIR filter N=1
                d.y = d.y *(1.0f-a) + da.y * (a);// FIR filter N=1
            }
        }
        
        Point2f c = Point2f((float)overlay.cols / 2.0f, (float)overlay.rows / 2.0f);
        Point2f cd = c + d*-60.0f;
        arrowedLine(overlay, c, cd, Scalar(255, 255, 255), 2, LINE_4, 0, 0.5);
        //circle(overlay,cf,10, Scalar( 255, 255, 255 ),FILLED,LINE_8 );
    }
    //cv::putText(overlay,std::to_string(_mean[0]) + ", " + std::to_string(_mean[1]),cv::Point(10, overlay.rows / 2),cv::FONT_HERSHEY_DUPLEX,1.0,CV_RGB(118, 185, 0),2);
    imshow("frame2", overlay);
}






int main(int argc, char const* argv[])
{
    printf("Hello! Openging %s\n", argv[1]);
    if(argv[1] == NULL) {return 0;}

    VideoCapture capture(samples::findFile(argv[1]));
    if (!capture.isOpened())
    {
        printf("Unable to open file! %s\n", argv[1]);
        return 0;
    }
    
    {
        int w = capture.get(CAP_PROP_FRAME_WIDTH);
        int h = capture.get(CAP_PROP_FRAME_HEIGHT);
        printf("Resolution %i %i\n", w, h);
    }

    Mat raw;
    Mat prvs;
    capture >> raw;
    if (raw.empty()) {return 0;}
    cvtColor(raw, prvs, COLOR_BGR2GRAY);

    while(true)
    {
        Mat next;
        capture >> raw;
        if (raw.empty())break;
        cvtColor(raw, next, COLOR_BGR2GRAY);

        go_with_the_flow(raw, next, prvs);

		
        int keyboard = waitKey(30);
        if (keyboard == 'q' || keyboard == 27)
            break;
            
        prvs = next;
    }
}