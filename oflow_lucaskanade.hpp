#include <opencv2/core/types_c.h>
using namespace cv;
using namespace std;

struct oflow_lucaskanade_context
{
    vector<Scalar> colors;
    RNG rng;
    Mat old_gray;
    vector<Point2f> p0, p1;
    Mat mask;
    int time;
};



void oflow_lucaskanade_init(struct oflow_lucaskanade_context * context, InputArray raw);


void oflow_lucaskanade_run(struct oflow_lucaskanade_context * context, InputOutputArray raw, Point2f& direction, float alpha);