#pragma once
#include <opencv2/core.hpp>

typedef struct
{
	bool help;
	bool ok;
	cv::Ptr<cv::CommandLineParser> parser;
	std::string filename;
} args_t;

void args_init(args_t &state, int argc, char **argv);