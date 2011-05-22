#pragma once
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
	GraphicsState& state;
	DeviceState& device;
	Buffer& gbuffer;
	ZBuffer& shadowmap;
	ZBuffer& shadowcube;
	Buffer& lbuffer;
	ZBuffer& zbuffer;
	Buffer& backbuffer;
	Camera& camera;

	CBuffer& cb_object;
	CBuffer& cb_object_z;
	CBuffer& cb_object_cube_z;
	CBuffer& cb_light;
	CBuffer& cb_frame;
	CBuffer& cb_tracy;

	void run();
};

} // namespace DEV
