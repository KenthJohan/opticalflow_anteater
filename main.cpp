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
#include "oflow.hpp"

using namespace cv;

// How many camera views and how its placed, like horizontally or verticly.
#define WELDVISI_VIEWS 3
#define WELDVISI_ROWS 1
#define WELDVISI_COLS 3

#define ASSETS_ABOUT \
"This sample demonstrates Optical Flow calculation.\n"

#define ASSETS_ARG_KEYS \
"{ h help |           | print this help message }" \
"{ @image | vtest.avi | path to image file }" \
"{ o      |           | Output file }"






// Sets multiple rectangles as a grid:
void rect_grid(Rect r[], int n, int rows, int cols, int w, int h)
{
    for(int y = 0; y < rows; ++y)
    {
        for(int x = 0; x < cols; ++x)
        {
            if (n == 0) {return;}
            n--;
            r[y*w + x].x = (x*w)/cols;
            r[y*w + x].y = (y*h)/rows;
            r[y*w + x].width = w/cols;
            r[y*w + x].height = h/rows;
        }
    }
}


// For visual purpose only:
// This draw the direction and speed:
void draw_arrow(Mat frame, Point2f direction)
{
    // Draw line from Point(c) to Point(c+d):
    // Where Point(c) is the center of frame:
    Point2f c = Point2f((float)frame.cols / 2.0f, (float)frame.rows / 2.0f);
    Point2f cd = c + direction;
    arrowedLine(frame, c, cd, Scalar(255, 255, 255), 2, LINE_4, 0, 0.5);
    char buf[100];
    float speed = HYPOT_F32(direction.x, direction.y);
    snprintf(buf, 100, "%5.2f", speed);
    //snprintf(buf, 100, "%+5.0f    ", (angle / M_PI) * 180.0f);
    cv::putText(frame, buf, cd, cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(0,255,0),2,false);
    //printf("%s\n", buf);
}





int main(int argc, char const* argv[])
{
    setbuf(stdout, NULL);
    printf("Hello this is the Anteater!\n");
    printf("cwd: '%s'\n", cv::utils::fs::getcwd().c_str());

    CommandLineParser parser(argc, argv, ASSETS_ARG_KEYS);
    VideoCapture capture;
    cv::String filename;
    Rect r[WELDVISI_VIEWS];
    Point2f direction[WELDVISI_VIEWS] = {};
    Mat raw;
    float alpha = 0.01f;
    float visual_direction_gain[WELDVISI_VIEWS] = {-60.0f, -120.0f, -60.0f};
    oflow_context context[WELDVISI_VIEWS];
    VideoWriter outputVideo;


    parser.about(ASSETS_ABOUT);
    if (parser.has("help"))
    {
        parser.printMessage();
        return 0;
    }

    filename = samples::findFile(parser.get<cv::String>("@image"));
    if (!parser.check())
    {
        parser.printErrors();
        return 0;
    }

    capture.open(filename);
    if (!capture.isOpened())
    {
        //error in opening the video input
        printf("Unable to open file! %s\n", filename.c_str());
        return 0;
    }

    {
        int w = capture.get(CAP_PROP_FRAME_WIDTH);
        int h = capture.get(CAP_PROP_FRAME_HEIGHT);
        printf("Resolution of videocapture is %ix%i!\n", w, h);
        rect_grid(r, WELDVISI_VIEWS, WELDVISI_ROWS, WELDVISI_COLS, w, h);
    }

    capture >> raw;
    for(int i = 0; i < WELDVISI_VIEWS; ++i)
    {
        oflow_init(context+i, raw(r[i]));
    }


    {
        String outname = parser.get<String>("o");
        if (outname.length() > 0)
        {
            int codec = static_cast<int>(capture.get(CAP_PROP_FOURCC));
            double fps = capture.get(CAP_PROP_FPS);
            bool isColor = (raw.type() == CV_8UC3);
            outputVideo.open(outname+filename, codec, fps, raw.size(), isColor);
        }
    }


    while(true)
    {
        {
            int keyboard = waitKey(30);
            if (keyboard == 'q' || keyboard == 27) {break;}
        }

        {
            capture >> raw;
            if (raw.empty()) {break;}
            for(int i = 0; i < WELDVISI_VIEWS; ++i)
            {
                oflow_run(context+i, raw(r[i]), direction[i], alpha);
                rectangle(raw, r[i], Scalar(0, 0, 255), 2, LINE_4);
                draw_arrow(raw(r[i]), direction[i]*visual_direction_gain[i]);
            }
            imshow(filename, raw);
            if (outputVideo.isOpened())
            {
                outputVideo.write(raw);
            }
        }
    }
    printf("Anteater exited successfully!\n");
    if (capture.isOpened()){capture.release();}   
    if (outputVideo.isOpened()) {outputVideo.release();}
    return 0;
}