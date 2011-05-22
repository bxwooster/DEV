#pragma once
#include "Task.hpp"
#include "Data/Camera.hpp"
#include "Data/GraphicsState.hpp"
#include "Data/RayTracingInfo.hpp"
#include "Data/ZBuffer.hpp"
#include "Data/Buffer.hpp"
#include "Data/CBuffer.hpp"

namespace DEV {

struct RayTrace
{
	InOut (GraphicsState) state;
	InOut (RayTracingInfo) info;
	In (Camera) camera;
	InOut (ZBuffer) zbuffer;
	InOut (Buffer) gbuffer;
	InOut (CBuffer) cb_frame;
	InOut (CBuffer) cb_tracy;

	void run();
};

} // namespace DEV