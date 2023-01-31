#include <stdio.h>



#include "mest.hpp"
#include "mest_visual.hpp"
#include "draw.hpp"
#include "histvel.hpp"
#include "histvel_visual.hpp"
#include "captures.hpp"
#include "args.hpp"
#include "userinputs.hpp"



#define CAM_WIDTH 1280
#define CAM_HEIGHT 720
#define MOTEST_COUNT 3
#define MOTEST_COUNT_MAX 3



int main(int argc, char **argv)
{
    setbuf(stdout, NULL);



    // Program arguments management
    args_t args1;
    args_init(args1, argc, argv);
    if (args1.ok == false){return 0;}



    // Video capture management
    captures_t captures;
    captures_init(captures, args1.filename);
    if (captures.capture.isOpened() == false)
    {
        fprintf(stderr, "Unable to open file!");
        return 1;
    }
    captures_progress(captures);



    // Motion estimation setup:
    motionest_state_t motest[MOTEST_COUNT_MAX];
    cv::Rect roi[MOTEST_COUNT_MAX];
    roi[0] = cv::Rect(400, 400, 200, 100);
    roi[1] = cv::Rect(1400, 400, 200, 100);
    roi[2] = cv::Rect(1810, 400, 200, 100);
    for(int i = 0; i < MOTEST_COUNT; ++i)
    {
        motionest_init(motest[i], captures.f1(roi[i]));
    }



    // Velocity histogram setup:
    histvel_state_t histvel;
    histvel_state_init(histvel);



    // Velocity histogram for debugging visual feedback:
    histvel_visual_t histvel_visual;
    histvel_visual_init(histvel_visual);



    //VideoWriter videowriter("Out.mp4", capture.get(CAP_PROP_FOURCC), capture.get(CAP_PROP_FPS), Size(capture.get(CAP_PROP_FRAME_WIDTH), capture.get(CAP_PROP_FRAME_HEIGHT)));

    userinputs_t userinputs;
    userinputs_init(userinputs);


    while(userinputs.should_exit == false)
    {
        userinputs_progress(userinputs);

        
	    captures.capture >> captures.frame;
        if (captures.frame.empty())
        {
            printf("Video ended!\n");
            captures_reset(captures);
            histvel_state_reset(histvel);
            histvel_visual_reset(histvel_visual);
            continue;
        }
        
        captures_progress(captures);

        
        histvel.histogram.setTo(0);
        for(int i = 0; i < MOTEST_COUNT; ++i)
        {
            // calculate optical flow
            motionest_progress(motest[i], captures.f1(roi[i]), captures.f2(roi[i]));
            histvel_state_add_flow(histvel, motest[i].flow);
        }
        
        histvel_state_progress(histvel);
        histvel_visual_draw(histvel_visual, histvel);


        mest_visual_draw(captures.frame, roi, motest, MOTEST_COUNT);
        
        
        //videowriter.write(img);


 
        
        
    }
}