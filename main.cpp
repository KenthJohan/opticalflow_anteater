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
#include "oflow.h"
#include "flecs.h"
#include "mainer.h"
#include "draw.h"

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








int main(int argc, char* argv[])
{
    ecs_world_t *world = ecs_init_w_args(argc, argv);
    mainer(world);

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
    float alpha = 0.1f;

    // Split a video frame into smaller views:
    Rect views[WELDVISI_VIEWS];

    // Speed and direction being set by the motion estimators:
    Vec2f32 direction[WELDVISI_VIEWS] = {};

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
        Mat sub = raw(views[i]);
        Vec2i32 resolution = {sub.cols, sub.rows};
        oflow_init(motion_estimator + i, sub.data, sub.type(), resolution);
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


 


    while(true)
    {
        ecs_progress(world, 0);

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
                Vec2i32 resolution = {raw.cols, raw.rows};
                Vec2i32 crop_pos = {views[i].x, views[i].y};
                Vec2i32 crop_size = {views[i].width, views[i].height};
                oflow_run(motion_estimator + i, raw.data, raw.type(), resolution, direction + i, alpha, crop_pos, crop_size);
            }
            
            for(int i = 0; i < WELDVISI_VIEWS; ++i)
            {
                Vec2i32 resolution = {raw.cols, raw.rows};
                Vec2i32 crop_pos = {views[i].x, views[i].y};
                Vec2i32 crop_size = {views[i].width, views[i].height};
                Vec2i32 rect_pos = {views[i].x, views[i].y};
                Vec2i32 rect_len = {views[i].width, views[i].height};
                // Developer feedback, Draw rectangle to visuale the views area for:
                draw_rectangle(raw.data, raw.type(), resolution, rect_pos, rect_len);
                // Developer feedback, Draw arrow for developer feedback:
                draw_arrow(raw.data, raw.type(), resolution, crop_pos, crop_size, direction[i], visual_direction_gain[i]);
            }

            // Developer feedback:
            {
                Vec2i32 resolution = {raw.cols, raw.rows};
                //draw_show(raw.data, raw.type(), resolution);
            }

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
    ecs_fini(world);
    return 0;
}