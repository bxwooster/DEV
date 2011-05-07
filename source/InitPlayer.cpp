#include "PlayerState.hpp"

#include <Windows.h>

namespace Devora {

void InitPlayer(PlayerState& state)
{
	state.camera.pitch = 0;
	state.camera.yaw = 0;
}

} // namespace Devora