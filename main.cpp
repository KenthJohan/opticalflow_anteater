#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include <opencv2/core/utils/filesystem.hpp>
#include <opencv2/core/types_c.h>
#include <stdio.h>
#include "common.h"
#include "oflow_farneback.hpp"

using namespace cv;

#define ASSETS_ABOUT \
"This sample demonstrates Lucas-Kanade Optical Flow calculation.\n" \
"The example file can be downloaded from:\n" \
"  https://www.bogotobogo.com/python/OpenCV_Python/images/mean_shift_tracking/slow_traffic_small.mp4"

#define ASSETS_ARG_KEYS \
"{ h help |      | print this help message }" \
"{ @image | vtest.avi | path to image file }"


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
    setbuf(stdout, NULL);
    printf("Hello this is the Anteater!\n");
    printf("cwd: '%s'\n", cv::utils::fs::getcwd().c_str());

    CommandLineParser parser(argc, argv, ASSETS_ARG_KEYS);
    parser.about(ASSETS_ABOUT);
    if (parser.has("help"))
    {
        parser.printMessage();
        return 0;
    }

    cv::String filename = samples::findFile(parser.get<cv::String>("@image"));
    if (!parser.check())
    {
        parser.printErrors();
        return 0;
    }

    VideoCapture capture(filename);
    if (!capture.isOpened())
    {
        //error in opening the video input
        printf("Unable to open file! %s\n", filename.c_str());
        return 0;
    }


    Rect r[NUM_OF_VIEWS];
    {
        int w = capture.get(CAP_PROP_FRAME_WIDTH);
        int h = capture.get(CAP_PROP_FRAME_HEIGHT);
        printf("Resolution of videocapture is %ix%i!\n", w, h);
        rect_init_three_way(r, w, h);
    }




    struct oflow_farneback_context context[NUM_OF_VIEWS];

    {
        Mat raw;
        capture >> raw;
        for(int i = 0; i < NUM_OF_VIEWS; ++i)
        {
            oflow_farneback_init(context+i, raw(r[i]));
        }
    }


    while(true)
    {
        int keyboard = waitKey(30);
        if (keyboard == 'q' || keyboard == 27) {break;}

        {
            Mat raw;
            capture >> raw;
            for(int i = 0; i < NUM_OF_VIEWS; ++i)
            {
                oflow_farneback_run(context+i, raw(r[i]));
                rectangle(raw, r[i], Scalar(255, 255, 255), 6, LINE_4);
            }
            imshow(filename, raw);
        }

    }
    printf("Anteater exited successfully!\n");
    return 0;

}