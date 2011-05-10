#include "PlayerState.hpp"
#include "TimingData.hpp"
#include "InputData.hpp"

#include <algorithm>

namespace Devora {

void DerivePlayerState(PlayerState& state, InputData& input, TimingData& timing)
{
	int const W = 87;
	int const S = 83;
	int const A = 65;
	int const D = 68;
	int const SPACE = 32;
	int const SHIFT = 16;

	auto end = input.keys_held.end();

	int const ESC = 27;
	if (input.keys_held.find(ESC) != end)
		throw 0;

	state.mov = Vector2f(0, 0);
	if (input.keys_held.find(W) != end)
		state.mov += Vector2f(-1, 0);
	if (input.keys_held.find(S) != end)
		state.mov += Vector2f(1, 0);
	if (input.keys_held.find(A) != end)
		state.mov += Vector2f(0, -1);
	if (input.keys_held.find(D) != end)
		state.mov += Vector2f(0, 1);

	if (state.mov != Vector2f(0, 0))
		state.mov.normalize();

	float yaw = float(state.camera.yaw / 180 * M_PI);
	Matrix2f rot;
	rot << cos(yaw), sin(yaw), -sin(yaw), cos(yaw);
	state.mov = rot * state.mov;

	state.jump = ( input.keys_held.find(SPACE) != end );
	state.sprint = ( input.keys_held.find(SHIFT) != end );

	state.camera.yaw += input.mouse.x * 0.25f; //! mouse sens
	state.camera.pitch += input.mouse.y * 0.25f;
	state.camera.pitch = std::max(-90.0f, std::min(state.camera.pitch, 90.0f));
}

} // namespace Devora