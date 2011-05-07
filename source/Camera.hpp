#ifndef __Devora_Camera__
#define __Devora_Camera__

#include "Matrix.h"

namespace Devora {

struct Camera
{
	Matrix4f view;
	Matrix4f proj;

	float vertical_fov;
	float aperture;
	float aspect_ratio;
	float z_near;

	struct
	{
		int w;
		int h;
	} screen;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

} // namespace Devora

#endif