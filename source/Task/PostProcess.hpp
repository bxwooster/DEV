#pragma once
#include "Task.hpp"
#include "Data/Camera.hpp"
#include "Data/GraphicsState.hpp"
#include "Data/PostProcessInfo.hpp"
#include "Data/ZBuffer.hpp"
#include "Data/Buffer.hpp"
#include "Data/CBuffer.hpp"

namespace DEV {

struct PostProcess
{
	InOut (GraphicsState) state;
	InOut (PostProcessInfo) info;
	InOut (ZBuffer) zbuffer;
	InOut (Buffer) gbuffer;
	InOut (Buffer) lbuffer;
	InOut (Buffer) backbuffer;
	InOut (CBuffer) cb_frame;

	void run();
};

} // namespace DEV