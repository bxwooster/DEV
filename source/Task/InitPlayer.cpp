#include "InitPlayer.hpp"
#include <Windows.h>

namespace DEV {

void InitPlayer::run()
{
	state.camera.pitch = 0;
	state.camera.yaw = 0;
}

} // namespace DEV