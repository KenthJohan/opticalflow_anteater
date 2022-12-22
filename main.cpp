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
#include "flecs.h"
#include "module_weldvisi.hpp"

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






// Make rectangles in a grid of rows and columns:
void rect_grid(Rect r[], int n, int rows, int cols, int w, int h)
{
    for(int y = 0; y < rows; ++y)
    {
        for(int x = 0; x < cols; ++x)
        {
            if (n == 0) {return;}
            n--;
            r[y*cols + x].x = (x*w)/cols;
            r[y*cols + x].y = (y*h)/rows;
            r[y*cols + x].width = w/cols;
            r[y*cols + x].height = h/rows;
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
    flecs::world ecs;
    ecs.import<simple::module>();

    setbuf(stdout, NULL);
    printf("Hello this is Anteater!\n");
    printf("cwd: '%s'\n", cv::utils::fs::getcwd().c_str());

    CommandLineParser parser(argc, argv, ASSETS_ARG_KEYS);

    // Video capture from the anteater:
    VideoCapture video_capture;

    // Video writing is optional for developer feedback:
    VideoWriter video_writer;

    // Input video file path:
    cv::String filename;

    // Raw video frame from the video capture:
    Mat raw;

    // FIR filter constant:
    float alpha = 0.01f;

    // Split a video frame into smaller views:
    Rect views[WELDVISI_VIEWS];

    // Speed and direction being set by the motion estimators:
    Point2f direction[WELDVISI_VIEWS] = {};

    // Make arrow longer for human friendly visuals:
    float visual_direction_gain[WELDVISI_VIEWS] = {-60.0f, -120.0f, -60.0f};

    // Motion estimators, direction and speed can be retrived from this:
    oflow_context motion_estimator[WELDVISI_VIEWS];


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

    video_capture.open(filename);
    if (!video_capture.isOpened())
    {
        //error in opening the video input
        printf("Unable to open file! %s\n", filename.c_str());
        return 0;
    }

    {
        int w = video_capture.get(CAP_PROP_FRAME_WIDTH);
        int h = video_capture.get(CAP_PROP_FRAME_HEIGHT);
        printf("Resolution of videocapture is %ix%i!\n", w, h);
        // Split video frame into smaller views:
        rect_grid(views, WELDVISI_VIEWS, WELDVISI_ROWS, WELDVISI_COLS, w, h);
    }


    // Init motion estimator with a starting frame:
    video_capture >> raw;
    for(int i = 0; i < WELDVISI_VIEWS; ++i)
    {
        oflow_init(motion_estimator + i, raw(views[i]));
    }


    {
        String outname = parser.get<String>("o");
        if (outname.length() > 0)
        {
            // Set the video writers to use the same settings as input video:
            int codec = static_cast<int>(video_capture.get(CAP_PROP_FOURCC));
            double fps = video_capture.get(CAP_PROP_FPS);
            bool isColor = (raw.type() == CV_8UC3);
            video_writer.open(outname+filename, codec, fps, raw.size(), isColor);
        }
    }

    flecs::entity e = ecs.entity()
        .set<simple::Position>({10, 20})
        .set<simple::Velocity>({1, 1});


    while(true)
    {
        ecs.progress();
        {
            int keyboard = waitKey(30);
            if (keyboard == 'q' || keyboard == 27) {break;}
        }

        {
            video_capture >> raw;
            if (raw.empty()) {break;}

            // Run motion estimation on every view:
            for(int i = 0; i < WELDVISI_VIEWS; ++i)
            {
                oflow_run(motion_estimator + i, raw(views[i]), direction[i], alpha);
                // Developer feedback, Draw rectangle to visuale the views area for:
                rectangle(raw, views[i], Scalar(0, 0, 255), 2, LINE_4);
                // Developer feedback, Draw arrow for developer feedback:
                draw_arrow(raw(views[i]), direction[i]*visual_direction_gain[i]);
            }

            // Developer feedback:
            imshow(filename, raw);

            // Developer feedback, Optional video writer:
            if (video_writer.isOpened())
            {
                video_writer.write(raw);
            }
        }
    }
    printf("Anteater exited successfully!\n");
    if (video_capture.isOpened()){video_capture.release();}   
    if (video_writer.isOpened()) {video_writer.release();}
    return 0;
}