#include "PlayerState.hpp"

#include <Windows.h>

namespace DEV {

void InitPlayer(PlayerState& state)
{
	state.camera.pitch = 0;
	state.camera.yaw = 0;
}

} // namespace DEV