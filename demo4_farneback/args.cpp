#include "args.hpp"

#include <opencv2/core.hpp>

void args_init(args_t &state, int argc, char **argv)
{
	state.ok = false;
	
    const std::string about =
        "This sample demonstrates Lucas-Kanade Optical Flow calculation.\n"
        "The example file can be downloaded from:\n"
        "  https://www.bogotobogo.com/python/OpenCV_Python/images/mean_shift_tracking/slow_traffic_small.mp4";
    const std::string keys =
        "{ h help |      | print this help message }"
        "{ @image | vtest.avi | path to image file }";
		
    state.parser = new cv::CommandLineParser(argc, argv, keys);
    state.parser->about(about);

    if (state.parser->has("help"))
    {
        state.parser->printMessage();
        return;
    }


    state.filename = cv::samples::findFile(state.parser->get<std::string>("@image"));
    if (!state.parser->check())
    {
        state.parser->printErrors();
        return;
    }

	state.ok = true;
}