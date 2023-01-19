#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include <assert.h>
using namespace cv;
using namespace std;

#define TRACKER_COUNT 100

void show_trackers(Mat m, vector<Point2f> p0, vector<Scalar> colors)
{
    for(uint i = 0; i < p0.size(); i++)
    {
        //line(m,p1[i], p0[i], colors[i], 2);
        circle(m, p0[i], 5, colors[i], -1);
    }
}

void show_traces(Mat m, vector<Point2f> p0, vector<Point2f> dir, vector<Scalar> colors)
{
    for(uint i = 0; i < p0.size(); i++)
    {
        Point2f p1 = p0[i] + dir[i];
        line(m, p0[i], p1, colors[i], 2);
    }
}



typedef struct
{
    vector<Scalar> colors;
    RNG rng;
    vector<uchar> status;
    vector<float> err;
    vector<Point2f> p0;
    vector<Point2f> p1;
    vector<Point2f> good_new;
    vector<Point2f> good_new_dir;
} motionest_state_t;

void motionest_init(motionest_state_t &state)
{
    for(int i = 0; i < TRACKER_COUNT; i++)
    {
        int r = state.rng.uniform(0, 256);
        int g = state.rng.uniform(0, 256);
        int b = state.rng.uniform(0, 256);
        state.colors.push_back(Scalar(r,g,b));
    }
}



void motionest_progress(motionest_state_t &state, InputArray f1, InputArray f2)
{
    {
        //cv::InputArray prevImg
        //cv::InputArray nextImg
        //cv::InputArray prevPts
        //cv::InputOutputArray nextPts
        //cv::OutputArray status
        //cv::OutputArray err
        cv::Size winSize = cv::Size(21, 21);
        int maxLevel = 3;
        TermCriteria criteria = TermCriteria((TermCriteria::COUNT) + (TermCriteria::EPS), 100, 10.0);
        int flags = 0;
        double minEigThreshold = 0.0001000000000000000048;
        assert(state.p0.size() > 0);
        calcOpticalFlowPyrLK(f1, f2, state.p0, state.p1, state.status, state.err, winSize, maxLevel, criteria, flags, minEigThreshold);
    }

    state.good_new.clear();
    state.good_new_dir.clear();
    for(uint i = 0; i < state.p0.size(); i++)
    {
        // Select good points
        if(state.status[i] != 1) {continue;}
        state.good_new.push_back(state.p1[i]);
        Point2f d = state.p1[i] - state.p0[i];
        state.good_new_dir.push_back(d);
    }

    state.p0 = state.good_new;
}







void motionest_progress_reset(motionest_state_t &state, InputArray f1)
{
    goodFeaturesToTrack(f1, state.p0, TRACKER_COUNT, 0.1, 13, Mat(), 3, false, 0.04);
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
    if (!capture.isOpened()){
        //error in opening the video input
        cerr << "Unable to open file!" << endl;
        return 0;
    }
    // Create some random colors


    motionest_state_t mot1;
    motionest_init(mot1);


    Mat frame;
    Mat f1;
    // Take first frame and find corners in it
    capture >> frame;
    cvtColor(frame, f1, COLOR_BGR2GRAY);
    // Create a mask image for drawing purposes
    Mat mask = Mat::zeros(frame.size(), frame.type());
    static int frame_index = 0;
    while(true)
    {
        {
            int keyboard = waitKey(30);
            if (keyboard == 'q' || keyboard == 27)
                break;
        }

        frame_index++;




        capture >> frame;
        printf("CAP_PROP_POS_FRAMES %i!\n", (int)capture.get(CAP_PROP_POS_FRAMES));
        if (frame.empty())
        {
            printf("Video ended!\n");
            capture.set(CAP_PROP_POS_FRAMES, 250);
            continue;
        }
        
        Mat f2;
        cvtColor(frame, f2, COLOR_BGR2GRAY);




        if((frame_index % 50) == 0)
        {
            motionest_progress_reset(mot1, f2);
            //mask = Mat::zeros(frame.size(), frame.type());
        }


        if(mot1.p0.size() > 0)
        {
            // calculate optical flow
            motionest_progress(mot1, f1, f2);
        }
 

        show_trackers(frame, mot1.good_new, mot1.colors);
        //show_traces(mask, mot1.good_new, mot1.good_new_dir, mot1.colors);
        Mat img;
        add(frame, mask, img);
        imshow("Frame", img);


        // Now update the previous frame and previous points
        f1 = f2.clone();
        
        
    }
}