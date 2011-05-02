#ifndef __Devora_Camera__
#define __Devora_Camera__

#include "Matrix.h"

namespace Devora {

struct Camera
{
	Matrix4f view;

	float field_of_view;
	float aspect_ratio;
	float aperture;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

} // namespace Devora

#endif