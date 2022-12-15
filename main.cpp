#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include <opencv2/core/types_c.h>
#include <stdio.h>
using namespace cv;



void go_with_the_flow(Mat frame2, Mat next, Mat prvs, float alpha)
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
        Point2f da = Point2f(0,0);
        for(int j = 0;j < flow.cols;j++){
            for(int i = 0;i < flow.rows;i++){
                da.x += flowp[0].at<float>(i,j);
                da.y += flowp[1].at<float>(i,j);
            }
        }
        da.x *= (1.0f/(flow.cols*flow.rows));
        da.y *= (1.0f/(flow.cols*flow.rows));

        
        // Direction average FIR calculation, FIR filter N=1
        static Point2f da_fir = Point2f(0,0);
        da_fir.x = da_fir.x *(1.0f-alpha) + da.x * (alpha);
        da_fir.y = da_fir.y *(1.0f-alpha) + da.y * (alpha);

        {
            //Draw line from (c) to (cd)
            Point2f c = Point2f((float)frame2.cols / 2.0f, (float)frame2.rows / 2.0f);
            Point2f cd = c + da_fir*-60.0f;
            arrowedLine(frame2, c, cd, Scalar(255, 255, 255), 2, LINE_4, 0, 0.5);
        }

        //circle(overlay,cf,10, Scalar( 255, 255, 255 ),FILLED,LINE_8 );
    }
    //cv::putText(overlay,std::to_string(_mean[0]) + ", " + std::to_string(_mean[1]),cv::Point(10, overlay.rows / 2),cv::FONT_HERSHEY_DUPLEX,1.0,CV_RGB(118, 185, 0),2);
    
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
    

    int w = capture.get(CAP_PROP_FRAME_WIDTH);
    int h = capture.get(CAP_PROP_FRAME_HEIGHT);
    printf("Resolution %i %i\n", w, h);


    Mat raw;
    Mat prvs;
    capture >> raw;
    if (raw.empty()) {return 0;}
    cvtColor(raw, prvs, COLOR_BGR2GRAY);

    while(true)
    {
        int keyboard = waitKey(30);
        if (keyboard == 'q' || keyboard == 27) {break;}

        Mat next;
        capture >> raw;
        if (raw.empty())break;
        cvtColor(raw, next, COLOR_BGR2GRAY);

        {
            Rect r[4] = 
            {
                Rect (0  , 0  , w/2, h/2),
                Rect (w/2, 0  , w/2, h/2),
                Rect (0  , h/2, w/2, h/2),
                Rect (w/2, h/2, w/2, h/2),
            };

            for (int i = 0; i < 4; ++i)
            {
                go_with_the_flow(raw(r[i]), next(r[i]), prvs(r[i]), 0.1f);   
            }


            imshow("cam2", raw);
        }

        prvs = next;
    }
}