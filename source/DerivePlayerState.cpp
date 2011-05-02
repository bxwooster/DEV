#include "PlayerState.hpp"
#include "TimingData.hpp"
#include "InputData.hpp"

#include "Matrix.h"
#include <math.h>

namespace Devora {

void DerivePlayerState(PlayerState& state, InputData& input, TimingData& timing)
{
	int const W = 87;
	int const S = 83;
	int const A = 65;
	int const D = 68;
	int const SPACE = 32;

	auto end = input.keys_held.end();

	state.mov = Vector2f(0, 0);
	if (input.keys_held.find(W) != end)
		state.mov += Vector2f(1, 0);
	if (input.keys_held.find(S) != end)
		state.mov += Vector2f(-1, 0);
	if (input.keys_held.find(A) != end)
		state.mov += Vector2f(0, 1);
	if (input.keys_held.find(D) != end)
		state.mov += Vector2f(0, -1);

	if (state.mov.x() != 0 || state.mov.y() != 0)
		state.mov.normalize();

	state.jump = ( input.keys_held.find(SPACE) != end );

	state.camera.yaw += input.mouse.x; //! mouse sens
	state.camera.pitch += input.mouse.y;
	//state.camera.pitch = max(-90, min(state.camera.pitch, 90)); //!
}

} // namespace Devora