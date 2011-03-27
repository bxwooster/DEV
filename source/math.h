#ifndef __common_math_h__
#define __common_math_h__

#define NOMINMAX
#define _USE_MATH_DEFINES
#include <math.h>

namespace common {   

const float to_radians = (float)(M_PI / 180.0);
const float from_radians = (float)(180.0 / M_PI);

} //namespace common

#endif