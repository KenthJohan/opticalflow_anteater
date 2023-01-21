#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>
#include <opencv2/features2d.hpp>


#include "mest.hpp"
#include "draw.hpp"

using namespace cv;
using namespace std;


















void motionest_init(motionest_state_t &state, cv::InputArray f1)
{
    state.flow = Mat(f1.size(), CV_32FC2);
    state.dir_fir = Point2f(0,0);
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



void motionest_progress(motionest_state_t &state, InputArray f1, InputArray f2)
{
    state.flow.setTo(Scalar(0.0, 0.0));
    calcOpticalFlowFarneback(f1, f2, state.flow, 0.5, 3, 15, 3, 5, 1.2, 0);

    state.dir = get_direction(state.flow);

	{
		int n = state.flow.rows * state.flow.cols;
	    float flow_factor = 1.0f / ((float)n);
    	state.dir *= flow_factor;
	}

    float alpha = 0.1f;
    state.dir_fir = state.dir_fir * (1.0f - alpha) + (state.dir * alpha);
}



