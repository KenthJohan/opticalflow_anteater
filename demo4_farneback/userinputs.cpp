#include "userinputs.hpp"
#include <opencv2/highgui.hpp>

void userinputs_init(userinputs_t &state)
{
	state.should_exit = false;
	state.play = false;
}

void userinputs_progress(userinputs_t &state)
{
	state.wait = state.play ? 30 : 0;

	state.key = cv::waitKey(state.wait);
	if (state.key == 'q' || state.key == 27)
	{
		state.should_exit = true;
	}

	if (state.key == 'p') 
	{
		state.play = !state.play;
	}


	//printf("Avg speed %f\n", histvel.speed_sum / histvel.speed_n);
}