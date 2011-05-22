#pragma once
#include "Task.hpp"
#include "Data/Camera.hpp"
#include "Data/DeviceState.hpp"
#include "Data/GraphicsState.hpp"
#include "Data/Buffer.hpp"
#include "Data/ZBuffer.hpp"
#include "Data/UBuffer.hpp"
#include "Data/CBuffer.hpp"

namespace DEV {

struct InitGraphics
{
	InOut (GraphicsState) state;
	InOut (DeviceState) device;
	InOut (Buffer) gbuffer;
	InOut (ZBuffer) shadowmap;
	InOut (ZBuffer) shadowcube;
	InOut (Buffer) lbuffer;
	InOut (ZBuffer) zbuffer;
	InOut (Buffer) backbuffer;
	InOut (Camera) camera;

	InOut (CBuffer) cb_object;
	InOut (CBuffer) cb_object_z;
	InOut (CBuffer) cb_object_cube_z;
	InOut (CBuffer) cb_light;
	InOut (CBuffer) cb_frame;
	InOut (CBuffer) cb_tracy;

	void run();
};

} // namespace DEV
