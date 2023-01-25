#include "captures.hpp"
#include <opencv2/imgproc.hpp>


void captures_reset(captures_t &state)
{
	state.capture.set(cv::CAP_PROP_POS_FRAMES, 270);
	state.capture >> state.frame;
	cv::cvtColor(state.frame, state.f1, cv::COLOR_BGR2GRAY);
	cv::cvtColor(state.frame, state.f2, cv::COLOR_BGR2GRAY);
}

void captures_init(captures_t &state, std::string filename)
{
	state.capture = cv::VideoCapture(filename);
	captures_reset(state);
	printf("CAP_PROP_FRAME_WIDTH  : %i\n", (int)state.capture.get(cv::CAP_PROP_FRAME_WIDTH));
    printf("CAP_PROP_FRAME_HEIGHT : %i\n", (int)state.capture.get(cv::CAP_PROP_FRAME_HEIGHT));
}





void captures_progress(captures_t &state)
{
	state.capture >> state.frame;
	state.frame_index++;
    state.f1 = state.f2.clone();
	cv::cvtColor(state.frame, state.f2, cv::COLOR_BGR2GRAY);
}