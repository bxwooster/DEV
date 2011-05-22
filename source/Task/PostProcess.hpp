#pragma once
#include "Data/Camera.hpp"
#include "Data/GraphicsState.hpp"
#include "Data/PostProcessInfo.hpp"
#include "Data/ZBuffer.hpp"
#include "Data/Buffer.hpp"
#include "Data/CBuffer.hpp"

namespace DEV {

struct PostProcess
{
	GraphicsState& state;
	PostProcessInfo& info;
	ZBuffer& zbuffer;
	Buffer& gbuffer;
	Buffer& lbuffer;
	Buffer& backbuffer;
	CBuffer& cb_frame;

	void run();
};

} // namespace DEV