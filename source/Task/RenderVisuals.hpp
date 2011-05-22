#pragma once
#include "Task.hpp"
#include "Data/Transforms.hpp"
#include "Data/GraphicsState.hpp"
#include "Data/VisualRenderInfo.hpp"
#include "Data/Visuals.hpp"
#include "Data/Geometries.hpp"
#include "Data/Camera.hpp"
#include "Data/ZBuffer.hpp"
#include "Data/Buffer.hpp"
#include "Data/UBuffer.hpp"
#include "Data/CBuffer.hpp"

namespace DEV {

struct RenderVisuals
{
	GraphicsState& state;
	VisualRenderInfo& info;
	Transforms& transforms;
	Visuals& visuals;
	Geometries& geometries;
	Camera& camera;
	UBuffer& oit_start;
	UBuffer& oit_scattered;
	UBuffer& oit_consolidated;
	Buffer& gbuffer;
	ZBuffer& zbuffer;
	CBuffer& cb_object;
	CBuffer& cb_frame;

	void run();
};

} // namespace DEV