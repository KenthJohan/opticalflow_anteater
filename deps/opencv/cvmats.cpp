#include "cvmats.hpp"
#include "types.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

cv::Mat mat2cvmat(Tensor2_U8C3 * mat)
{
    int sizes[] = {(int)mat->shape[0], (int)mat->shape[1]};
    size_t steps[] = {(size_t)mat->step[0], (size_t)mat->step[1]};
    cv::Mat m = cv::Mat(mat->dims, sizes, mat->type, mat->start, steps);
    return m;
}