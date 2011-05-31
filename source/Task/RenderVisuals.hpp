#pragma once
#include "Tasking.hpp"
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
	InOut (GraphicsState) state;
	InOut (VisualRenderInfo) info;
	In (Transforms) transforms;
	In (Visuals) visuals;
	InOut (Geometries) geometries;
	In (Camera) camera;
	InOut (UBuffer) oit_start;
	InOut (UBuffer) oit_scattered;
	InOut (UBuffer) oit_consolidated;
	InOut (Buffer) gbuffer;
	InOut (ZBuffer) zbuffer;
	InOut (CBuffer) cb_object;
	InOut (CBuffer) cb_frame;

	__End__;

	void run();
};

} // namespace DEV