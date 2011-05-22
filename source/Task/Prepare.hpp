#pragma once
#include "Data/Camera.hpp"
#include "Data/GraphicsState.hpp"
#include "Data/ZBuffer.hpp"
#include "Data/CBuffer.hpp"
#include "Data/UBuffer.hpp"

namespace DEV {

struct Prepare
{
	GraphicsState& state;
	CBuffer& cb_frame;
	ZBuffer& zbuffer;
	UBuffer& oit_start;
	Camera& camera;

	void run();
};

} // namespace DEV