#pragma once
#include "Data/Camera.hpp"
#include "Data/DeviceState.hpp"
#include "Data/GraphicsState.hpp"
#include "Data/Buffer.hpp"
#include "Data/ZBuffer.hpp"
#include "Data/UBuffer.hpp"

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

	void run();
};

} // namespace DEV
