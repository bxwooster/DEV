#pragma once
#include "Tasking.hpp"
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

	__End__;

	void run();

private:
	struct Parameters
	{
		btAlignedObjectArray<Light> const& lights;
		ZBuffer& shadowsurface;
		ID3D11VertexShader* vs_render;
		ID3D11GeometryShader* gs_render;
		CBuffer& cbuffer;
		ID3D11GeometryShader* gs_light;
		ID3D11PixelShader* ps_light;
	};

	inline void shared_code(Parameters& par);
};

} // namespace DEV