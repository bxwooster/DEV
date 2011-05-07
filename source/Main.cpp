#define NOMINMAX


#include "DeviceState.hpp"
#include "GraphicsState.hpp"
#include "InputData.hpp"
#include "TimingData.hpp"
#include "LightRenderInfo.hpp"
#include "PostProcessInfo.hpp"
#include "RayTracingInfo.hpp"
#include "VisualRenderInfo.hpp"
#include "PhysicsState.hpp"
#include "PlayerState.hpp"
#include "Camera.hpp"
#include "Transforms.hpp"
#include "Visuals.hpp"
#include "Lights.hpp"
#include "Geometries.hpp"
#include "Buffer.hpp"
#include "ZBuffer.hpp"
#include "CBuffer.hpp"

#include <exception>
#include <Windows.h>

namespace Devora {

void InitGraphics(GraphicsState& state, DeviceState& device,
	VisualRenderInfo& vinfo, LightRenderInfo& linfo, PostProcessInfo& pinfo, RayTracingInfo& rinfo,
	Buffer& gbuffer0, Buffer& gbuffer1, ZBuffer& shadowmap, ZBuffer& shadowcube,
	Buffer& lbuffer, ZBuffer& zbuffer, Buffer& backbuffer, Camera& camera,
	CBuffer& cb_object, CBuffer& cb_object_z, CBuffer& cb_object_cube_z,
	CBuffer& cb_light, CBuffer& cb_frame, CBuffer& cb_tracy);

void InitInput(InputData& input);
void GetInput(InputData& input);

void InitTiming(TimingData& timing);
void GetTiming(TimingData& timing);

void InitScene(Transforms& transforms, Visuals& visuals, Lights& lights,
	Geometries& geometries, PhysicsState& state, DeviceState& device);

void InitPhysics(PhysicsState& state);
void CrunchPhysics(PhysicsState& state, Transforms& transforms,
	PlayerState& player, TimingData& timing);

void DerivePlayerState(PlayerState& state, InputData& input, TimingData& timing);
void DeriveCamera(Transforms& transforms, PlayerState& player, Camera& camera);
void Present(DeviceState& state);
void Prepare(GraphicsState& state, CBuffer& cb_frame, ZBuffer& zbuffer, Camera& camera);

void RenderVisuals(GraphicsState& state, VisualRenderInfo& info, 
	Transforms& transforms, Visuals& visuals, Geometries& geometries, Camera& camera,
	Buffer& gbuffer0, Buffer& gbuffer1, ZBuffer& zbuffer, CBuffer& cb_object);

void RenderLights(GraphicsState& state, LightRenderInfo& info,
	Transforms& transforms, Lights& lights, Visuals& casters, Geometries& geometries, 
	Camera& camera,	ZBuffer& zbuffer, ZBuffer& shadowmap, ZBuffer& shadowcube,
	Buffer& gbuffer0, Buffer& gbuffer1, Buffer& lbuffer,
	CBuffer& cb_frame, CBuffer& cb_object_z, CBuffer& cb_object_cube_z, CBuffer& cb_light);

void PostProcess(GraphicsState& state, PostProcessInfo& info, ZBuffer& zbuffer,
	Buffer& gbuffer0, Buffer& gbuffer1,	Buffer& lbuffer, Buffer& backbuffer, CBuffer& cb_frame);

void RayTrace(GraphicsState& state, RayTracingInfo& info,
	Camera& camera,	ZBuffer& zbuffer, Buffer& gbuffer0, Buffer& gbuffer1,
	CBuffer& cb_frame, CBuffer& cb_tracy);


void run()
{
	// Data
	GraphicsState graphics;
	DeviceState device;
	PhysicsState physics;

	VisualRenderInfo vinfo;
	LightRenderInfo linfo;
	PostProcessInfo pinfo;
	RayTracingInfo rinfo;

	TimingData timing;
	InputData input;
	Camera camera;
	PlayerState player;

	Buffer backbuffer, gbuffer0, gbuffer1, lbuffer;
	ZBuffer zbuffer, shadowmap, shadowcube;

	Transforms transforms;
	Visuals visuals;
	Lights lights;
	Geometries geometries;

	CBuffer cb_object, cb_object_z, cb_object_cube_z, cb_light, cb_frame, cb_tracy;

	// Code
	InitGraphics(graphics, device, vinfo, linfo, pinfo, rinfo, gbuffer0, gbuffer1,
		shadowmap, shadowcube, lbuffer, zbuffer, backbuffer, camera,
		cb_object, cb_object_z, cb_object_cube_z, cb_light, cb_frame, cb_tracy);
	InitPhysics(physics);
	InitTiming(timing);
	InitInput(input);
	InitScene(transforms, visuals, lights, geometries, physics, device);

	for (;;)
	{
		GetTiming(timing);
		GetInput(input);
		DerivePlayerState(player, input, timing);
		CrunchPhysics(physics, transforms, player, timing);
		DeriveCamera(transforms, player, camera);

		Prepare(graphics, cb_frame, zbuffer, camera);
		RenderVisuals(graphics, vinfo, transforms, visuals, geometries,
			camera, gbuffer0, gbuffer1, zbuffer, cb_object);
		RayTrace(graphics, rinfo, camera, zbuffer, gbuffer0, gbuffer1, cb_frame, cb_tracy);
		RenderLights(graphics, linfo, transforms, lights, visuals, geometries,
			camera, zbuffer, shadowmap, shadowcube, gbuffer0, gbuffer1, lbuffer,
			cb_frame, cb_object_z, cb_object_cube_z, cb_light);
		PostProcess(graphics, pinfo, zbuffer, gbuffer0, gbuffer1, lbuffer, backbuffer, cb_frame);
		Present(device);
	}
}


} // namespace Devora

int main()
{
	try
	{
		Devora::run();
	}
	catch(std::exception exception)
	{
		MessageBox( GetConsoleWindow(), exception.what(), "Exception!", 0);
		return -1;
	}
	catch(int status) 
	{
		return status;
	}
}