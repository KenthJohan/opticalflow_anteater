#include "histvel_visual.hpp"
#include "histvel.hpp"
#include "draw.hpp"
#include <opencv2/plot.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>




void histvel_visual_init(histvel_visual_t &visual)
{
    visual.speed_n = 0;
    visual.speed_sum = 0;
    //visual.vels.push_back(0);
}



void histvel_visual_reset(histvel_visual_t &visual)
{
	visual.vels.clear();
    visual.speed_n = 0;
    visual.speed_sum = 0;
}




#define UPSCALE 2



// Temorary stuff:
#define MATERIAL_LENGTH 0.25
#define WELDING_DURATION 12.0
#define WELD_SPEED (MATERIAL_LENGTH/WELDING_DURATION)
#define WELD_PIX_AVG_SPEED 2.7
#define THE_FACTOR (WELD_SPEED / WELD_PIX_AVG_SPEED)


void histvel_visual_draw(histvel_visual_t &visual, histvel_state_t &state)
{
    cv::Mat visual_histogram;


	{
	    cv::Mat bgr;
	    cvtColor(state.histogram, bgr, cv::COLOR_GRAY2BGR);
	    draw_resize(bgr, visual_histogram, UPSCALE);
	}


	{
	    draw_crosshair(visual_histogram);
	    draw_direction(visual_histogram, state.dir[0]*UPSCALE*HVEL_GAIN, CV_RGB(255, 0, 0));
	    //draw_direction(big, state.dir[1]*UPSCALE*HVEL_GAIN, CV_RGB(0, 255, 0));
	    //draw_direction(big, state.dir[2]*UPSCALE*HVEL_GAIN, CV_RGB(0, 0, 255));
	    draw_circle_center(visual_histogram, state.ignore_radius*UPSCALE);
	    draw_text_float(visual_histogram, cv::Point(visual_histogram.cols - UPSCALE*50, UPSCALE*(10+5)), CV_RGB(255, 255, 255), UPSCALE*0.2, "ref %7.4f m/s", WELD_SPEED);
	    draw_text_float(visual_histogram, cv::Point(visual_histogram.cols - UPSCALE*50, UPSCALE*(10+10)), CV_RGB(255, 0, 0), UPSCALE*0.2, " f0 %7.4f m/s", state.mag0*THE_FACTOR);
	    draw_text_float(visual_histogram, cv::Point(visual_histogram.cols - UPSCALE*50, UPSCALE*(10+15)), CV_RGB(0, 255, 0), UPSCALE*0.2, " f1 %7.4f m/s", state.mag1*THE_FACTOR);
	    draw_text_float(visual_histogram, cv::Point(visual_histogram.cols - UPSCALE*50, UPSCALE*(10+20)), CV_RGB(0, 0, 255), UPSCALE*0.2, " f2 %7.4f m/s", state.mag2*THE_FACTOR);
	    cv::imshow("Velocities Histogram", visual_histogram);
	}


	{
	    visual.speed_sum += state.mag1;
	    visual.speed_n += 1;
	}


	{
    	visual.vels.push_back(state.mag2);
	}


	{
	    cv::Mat display;
	    visual.plot = cv::plot::Plot2d::create(visual.vels);
	    visual.plot->setInvertOrientation(true);
	    visual.plot->setShowText(false);
	    visual.plot->render(display);
	    cv::imshow("Graph Plot", display);
	}




}