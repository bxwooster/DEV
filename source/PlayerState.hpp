#ifndef __Devora_PlayerState__
#define __Devora_PlayerState__

#include "Matrix.h"

namespace Devora {

struct PlayerState
{
	struct
	{
		float yaw;
		float pitch;
	} camera;
	Vector2f mov;
	bool jump;
};

} // namespace Devora

#endif