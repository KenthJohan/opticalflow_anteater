#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include <opencv2/features2d.hpp>
#include <assert.h>

#include "mest.hpp"
#include "draw.hpp"
#include "histvel.hpp"


using namespace cv;
using namespace std;





#define CAM_WIDTH 1280
#define CAM_HEIGHT 720
#define MOTEST_COUNT 3
#define MOTEST_COUNT_MAX 3





int main(int argc, char **argv)
{
    setbuf(stdout, NULL);
    const string about =
        "This sample demonstrates Lucas-Kanade Optical Flow calculation.\n"
        "The example file can be downloaded from:\n"
        "  https://www.bogotobogo.com/python/OpenCV_Python/images/mean_shift_tracking/slow_traffic_small.mp4";
    const string keys =
        "{ h help |      | print this help message }"
        "{ @image | vtest.avi | path to image file }";
    CommandLineParser parser(argc, argv, keys);
    parser.about(about);
    if (parser.has("help"))
    {
        parser.printMessage();
        return 0;
    }
    string filename = samples::findFile(parser.get<string>("@image"));
    if (!parser.check())
    {
        parser.printErrors();
        return 0;
    }
    VideoCapture capture(filename);
    capture.set(CAP_PROP_POS_FRAMES, 220);
    printf("CAP_PROP_FRAME_WIDTH  : %i\n", (int)capture.get(CAP_PROP_FRAME_WIDTH));
    printf("CAP_PROP_FRAME_HEIGHT : %i\n", (int)capture.get(CAP_PROP_FRAME_HEIGHT));

    if (!capture.isOpened())
    {
        //error in opening the video input
        cerr << "Unable to open file!" << endl;
        return 0;
    }
    // Create some random colors



    motionest_state_t motest[MOTEST_COUNT_MAX];
    Rect roi[MOTEST_COUNT_MAX];

    roi[0] = Rect(400, 400, 200, 100);
    roi[1] = Rect(1400, 400, 200, 100);
    roi[2] = Rect(1810, 400, 200, 100);

    //roi[0] = Rect(0, 0, 400, 200);
    //roi[1] = Rect(1400, 50, 400, CAM_HEIGHT-150);
    //roi[2] = Rect(1810, 50, 400, CAM_HEIGHT-200);


    histvel_state_t histvel;
    histvel_init(histvel);




    Mat frame;
    Mat f1;
    Mat f2;
    static int frame_index = 0;


    capture >> frame;
    cvtColor(frame, f1, COLOR_BGR2GRAY);
    cvtColor(frame, f2, COLOR_BGR2GRAY);

    for(int i = 0; i < MOTEST_COUNT; ++i)
    {
        motionest_init(motest[i], f1(roi[i]));
    }


    //VideoWriter videowriter("Out.mp4", capture.get(CAP_PROP_FOURCC), capture.get(CAP_PROP_FPS), Size(capture.get(CAP_PROP_FRAME_WIDTH), capture.get(CAP_PROP_FRAME_HEIGHT)));

    int wait_delay = 0;
    while(true)
    {

        {
            int keyboard = waitKey(wait_delay);
            if (keyboard == 'q' || keyboard == 27) {break;}
            if (keyboard == 'p') 
            {
                wait_delay = (wait_delay == 0) ? 30 : 0;
            }
            printf("Avg speed %f\n", histvel.speed_sum / histvel.speed_n);
        }



        frame_index++;




        capture >> frame;
        //printf("CAP_PROP_POS_FRAMES %i!\n", (int)capture.get(CAP_PROP_POS_FRAMES));
        if (frame.empty())
        {
            printf("Video ended!\n");
            capture.set(CAP_PROP_POS_FRAMES, 270);
            histvel.speed_sum = 0;
            histvel.speed_n = 0;
            continue;;
        }
        
        cvtColor(frame, f2, COLOR_BGR2GRAY);


        
        histvel.histogram.setTo(0);
        for(int i = 0; i < MOTEST_COUNT; ++i)
        {
            // calculate optical flow
            motionest_progress(motest[i], f1(roi[i]), f2(roi[i]));
            histvel_add_flow(histvel, motest[i].flow);
        }
        
        histvel_progress(histvel);
        histvel_draw(histvel);


        for(int i = 0; i < MOTEST_COUNT; ++i)
        {
            draw_direction(frame(roi[i]), motest[i].dir_fir * 50.0f, cv::Scalar(200, 10, 50));
            rectangle(frame, roi[i], Scalar(0, 0, 255), 4);
        }

        imshow("Frame", frame);
        show_flow(motest[0].flow);
        
        
        //videowriter.write(img);


        // Now update the previous frame and previous points
        f1 = f2.clone();
        
        
    }
}