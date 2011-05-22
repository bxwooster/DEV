#pragma once
#include "Task.hpp"
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
	InOut (GraphicsState) state;
	InOut (LightRenderInfo) info;
	In (Transforms) transforms;
	In (Lights) lights;
	In (Visuals) casters;
	InOut (Geometries) geometries;
	In (Camera) camera;
	InOut (ZBuffer) zbuffer;
	InOut (ZBuffer) shadowmap;
	InOut (ZBuffer) shadowcube;
	InOut (Buffer) gbuffer;
	InOut (Buffer) lbuffer;
	InOut (UBuffer) oit_start;
	InOut (UBuffer) oit_scattered;
	InOut (UBuffer) oit_consolidated;
	InOut (CBuffer) cb_frame;
	InOut (CBuffer) cb_object_z;
	InOut (CBuffer) cb_object_cube_z;
	InOut (CBuffer) cb_light;

	void run();
};

} // namespace DEV