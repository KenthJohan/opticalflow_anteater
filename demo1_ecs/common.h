#pragma once

#define HYPOT_F64(x,y) (sqrt((x)*(x) + (y)*(y)))
#define HYPOT_F32(x,y) (sqrtf((x)*(x) + (y)*(y)))
#define EG_CLAMP(x, x0, x1) (((x) > (x1)) ? (x1) : (((x) < (x0)) ? (x0) : (x)))