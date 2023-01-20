#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include <assert.h>
using namespace cv;
using namespace std;


#define CAM_WIDTH 1280
#define CAM_HEIGHT 720
#define MOTEST_COUNT 1
#define MOTEST_COUNT_MAX 3


void draw_direction(Mat m, Point2f dir)
{
    Point2f o1 = Point2f(m.cols / 2, m.rows / 2);
    Point2f o2 = o1 + dir;
    arrowedLine(m, o1, o2, Scalar(255, 0, 0), 2);
}

Point2f get_direction(Mat m)
{
    float dx = 0;
    float dy = 0;
    for (MatIterator_<Vec2f> it = m.begin<Vec2f>(); it != m.end<Vec2f>(); ++it)
    {
        float dx0 = (*it)[0];
        float dy0 = (*it)[1];
        dx += dx0;
        dy += dy0;
    }
    return Point2f(dx, dy);
}


void histo(Mat m, Mat out)
{
    float dx = 0;
    float dy = 0;
    for (MatIterator_<Vec2f> it = m.begin<Vec2f>(); it != m.end<Vec2f>(); ++it)
    {
        float x = (*it)[0];
        float y = (*it)[1];
        if (x == 0){continue;}
        if (y == 0){continue;}
        if (isnanf(x)){continue;}
        if (isnanf(y)){continue;}
        if (isinf(x)){continue;}
        if (isinf(y)){continue;}
        x *= 1000.0f;
        y *= 1000.0f;
        x += out.cols/2;
        y += out.rows/2;
        if(x >= out.cols) {continue;}
        if(y >= out.rows) {continue;}
        if(x < 0) {continue;}
        if(y < 0) {continue;}
        out.at<float>(x, y) += 1.0f;
    }
}


typedef struct
{
    Mat flow;
    Point2f dir;
    Point2f dir_fir;
    float flow_factor;
    Mat h;
} motionest_state_t;

void motionest_init(motionest_state_t &state, InputArray f1)
{
    state.h = Mat(Size(500, 500), CV_32F);
    state.flow = Mat(f1.size(), CV_32FC2);
    state.dir_fir = Point2f(0,0);
    int n = state.flow.rows * state.flow.cols;
    state.flow_factor = 1.0f / ((float)n);
}


void motionest_progress(motionest_state_t &state, InputArray f1, InputArray f2)
{
    state.h.setTo(0);
    state.flow.setTo(Scalar(0.0, 0.0));
    calcOpticalFlowFarneback(f1, f2, state.flow, 0.5, 3, 15, 3, 5, 1.2, 0);
    histo(state.flow, state.h);
    imshow("Hist", state.h);

    {
        double vmax;
        double vmin;
        Point imax;
        Point imin;
        minMaxLoc(state.h, &vmin, &vmax, &imin, &imax);
        imax.x -= state.h.cols / 2;
        imax.y -= state.h.rows / 2;
        printf("%f, %i, %i\n", vmax, imax.x, imax.y);
    }



    /**/
    state.dir = get_direction(state.flow);
    state.dir *= state.flow_factor;
    float alpha = 0.4f;
    state.dir_fir = state.dir_fir * (1.0f - alpha) + (state.dir * alpha);
    // compute sum of positive matrix elements
    // (assuming that M isa double-precision matrix)



    // visualization

    printf("flow_parts\n");


}


void show_flow(Mat flow)
{
    Mat bgr; //Visual
    Mat _hsv[3], hsv, hsv8;
    Mat flow_parts[2];
    Mat magnitude, angle, magn_norm;
    split(flow, flow_parts);
    cartToPolar(flow_parts[0], flow_parts[1], magnitude, angle, true);
    normalize(magnitude, magn_norm, 0.0f, 1.0f, NORM_MINMAX);
    angle *= ((1.f / 360.f) * (180.f / 255.f));
    _hsv[0] = angle;
    _hsv[1] = Mat::ones(angle.size(), CV_32F);
    _hsv[2] = magn_norm;
    merge(_hsv, 3, hsv);
    hsv.convertTo(hsv8, CV_8U, 255.0);
    cvtColor(hsv8, bgr, COLOR_HSV2BGR);
    imshow("flow", bgr);
}



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
    capture.set(CAP_PROP_POS_FRAMES, 250);
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
    //roi[0] = Rect(0, 0, 400, 200);
    roi[1] = Rect(1400, 50, 400, CAM_HEIGHT-150);
    roi[2] = Rect(1810, 50, 400, CAM_HEIGHT-200);


    


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


    while(true)
    {
        {
            int keyboard = waitKey(30);
            if (keyboard == 'q' || keyboard == 27)
                break;
        }

        frame_index++;




        capture >> frame;
        //printf("CAP_PROP_POS_FRAMES %i!\n", (int)capture.get(CAP_PROP_POS_FRAMES));
        if (frame.empty())
        {
            printf("Video ended!\n");
            capture.set(CAP_PROP_POS_FRAMES, 270);
            continue;;
        }
        
        cvtColor(frame, f2, COLOR_BGR2GRAY);

        for(int i = 0; i < MOTEST_COUNT; ++i)
        {
            // calculate optical flow
            motionest_progress(motest[i], f1(roi[i]), f2(roi[i]));
        }


        for(int i = 0; i < MOTEST_COUNT; ++i)
        {
            draw_direction(frame(roi[i]), motest[i].dir_fir * 50.0f);
            rectangle(frame, roi[i], Scalar(0, 0, 255), 4);
        }

        imshow("Frame", frame);
        show_flow(motest[0].flow);
        
        
        //videowriter.write(img);


        // Now update the previous frame and previous points
        f1 = f2.clone();
        
        
    }
}