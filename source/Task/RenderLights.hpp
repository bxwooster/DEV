#pragma once
#include "Data/Transforms.hpp"
#include "Data/GraphicsState.hpp"
#include "Data/LightRenderInfo.hpp"
#include "Data/Lights.hpp"
#include "Data/Visuals.hpp"
#include "Data/Geometries.hpp"
#include "Data/Camera.hpp"
#include "Data/ZBuffer.hpp"
#include "Data/Buffer.hpp"
#include "Data/CBuffer.hpp"
#include "Data/UBuffer.hpp"

namespace DEV {

struct RenderLights
{
	GraphicsState& state;
	LightRenderInfo& info;
	Transforms& transforms;
	Lights& lights;
	Visuals& casters;
	Geometries& geometries;
	Camera& camera;
	ZBuffer& zbuffer;
	ZBuffer& shadowmap;
	ZBuffer& shadowcube;
	Buffer& gbuffer;
	Buffer& lbuffer;
	UBuffer& oit_start;
	UBuffer& oit_scattered;
	UBuffer& oit_consolidated;
	CBuffer& cb_frame;
	CBuffer& cb_object_z;
	CBuffer& cb_object_cube_z;
	CBuffer& cb_light;

	void run();
};

} // namespace DEV