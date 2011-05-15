#pragma once

#include "Matrix.h"

namespace DEV {

struct PlayerState
{
	struct
	{
		float yaw;
		float pitch;
	} camera;
	Vector2f mov;
	bool jump;
	bool sprint;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

} // namespace DEV
