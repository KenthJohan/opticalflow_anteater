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
#include "histvel_visual.hpp"
#include "captures.hpp"


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
        return 1;
    }


    captures_t captures;
    captures_init(captures, filename);
    if (captures.capture.isOpened() == false)
    {
        fprintf(stderr, "Unable to open file!");
        return 1;
    }


    motionest_state_t motest[MOTEST_COUNT_MAX];
    Rect roi[MOTEST_COUNT_MAX];

    roi[0] = Rect(400, 400, 200, 100);
    roi[1] = Rect(1400, 400, 200, 100);
    roi[2] = Rect(1810, 400, 200, 100);

    //roi[0] = Rect(0, 0, 400, 200);
    //roi[1] = Rect(1400, 50, 400, CAM_HEIGHT-150);
    //roi[2] = Rect(1810, 50, 400, CAM_HEIGHT-200);


    histvel_state_t histvel;
    histvel_state_init(histvel);

    histvel_visual_t histvel_visual;
    histvel_visual_init(histvel_visual);

    captures_progress(captures);
    for(int i = 0; i < MOTEST_COUNT; ++i)
    {
        motionest_init(motest[i], captures.f1(roi[i]));
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
            //printf("Avg speed %f\n", histvel.speed_sum / histvel.speed_n);
        }



        captures_progress(captures);
        if (captures.frame.empty())
        {
            printf("Video ended!\n");
            captures_reset(captures);
            histvel_state_reset(histvel);
            histvel_visual_reset(histvel_visual);
            continue;;
        }

        
        histvel.histogram.setTo(0);
        for(int i = 0; i < MOTEST_COUNT; ++i)
        {
            // calculate optical flow
            motionest_progress(motest[i], captures.f1(roi[i]), captures.f2(roi[i]));
            histvel_state_add_flow(histvel, motest[i].flow);
        }
        
        histvel_state_progress(histvel);
        histvel_visual_draw(histvel_visual, histvel);


        for(int i = 0; i < MOTEST_COUNT; ++i)
        {
            draw_direction(captures.frame(roi[i]), motest[i].dir_fir * 50.0f, cv::Scalar(200, 10, 50));
            rectangle(captures.frame, roi[i], Scalar(0, 0, 255), 4);
        }

        imshow("Frame", captures.frame);
        show_flow(motest[0].flow);
        
        
        //videowriter.write(img);


 
        
        
    }
}