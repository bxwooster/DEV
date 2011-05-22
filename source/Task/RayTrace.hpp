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
	GraphicsState& state;
	RayTracingInfo& info;
	Camera& camera;
	ZBuffer& zbuffer;
	Buffer& gbuffer;
	CBuffer& cb_frame;
	CBuffer& cb_tracy;

	void run();
};

} // namespace DEV