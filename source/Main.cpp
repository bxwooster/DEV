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
#include "UBuffer.hpp"
#include "CBuffer.hpp"
#include "CBufferLayouts.hpp"
#include "ShaderCache.hpp"

#include <exception>
#include <Windows.h>

namespace DEV {

void InitOIT(DeviceState& device, Camera& camera,
	UBuffer& oit_start, UBuffer& oit_scattered,	UBuffer& oit_consolidated);

void InitGraphics(GraphicsState& state, DeviceState& device, 
	Buffer& gbuffer0, Buffer& gbuffer1, ZBuffer& shadowmap, ZBuffer& shadowcube,
	Buffer& lbuffer, ZBuffer& zbuffer, Buffer& backbuffer, Camera& camera);

void InitCBuffer(DeviceState& device, CBuffer& cb, size_t size);

void InitVisualRender(VisualRenderInfo& info, DeviceState& device, ShaderCache& cache);
void InitLightRender(LightRenderInfo& info, DeviceState& device, ShaderCache& cache, Camera& camera);
void InitPostProcess(PostProcessInfo& info, DeviceState& device, ShaderCache& cache);
void InitRayTracing(RayTracingInfo& info, DeviceState& device, ShaderCache& cache);

void InitPlayer(PlayerState& state);
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
void Prepare(GraphicsState& state, CBuffer& cb_frame, ZBuffer& zbuffer,
	UBuffer& oit_start, Camera& camera);

void RenderVisuals(GraphicsState& state, VisualRenderInfo& info, 
	Transforms& transforms, Visuals& visuals, Geometries& geometries, Camera& camera,
	UBuffer& oit_start, UBuffer& oit_scattered, UBuffer& oit_consolidated,
	Buffer& gbuffer0, Buffer& gbuffer1, ZBuffer& zbuffer, CBuffer& cb_object, CBuffer& cb_frame);

void RenderLights(GraphicsState& state, LightRenderInfo& info,
	Transforms& transforms, Lights& lights, Visuals& casters, Geometries& geometries, 
	Camera& camera,	ZBuffer& zbuffer, ZBuffer& shadowmap, ZBuffer& shadowcube,
	Buffer& gbuffer0, Buffer& gbuffer1, Buffer& lbuffer,
	UBuffer& oit_start, UBuffer& oit_consolidated,
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
	ShaderCache shadercache;

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
	UBuffer oit_start, oit_scattered, oit_consolidated;

	Transforms transforms;
	Visuals visuals;
	Lights lights;
	Geometries geometries;

	CBuffer cb_object, cb_object_z, cb_object_cube_z, cb_light, cb_frame, cb_tracy;

	// Code
	InitGraphics(graphics, device, gbuffer0, gbuffer1,
		shadowmap, shadowcube, lbuffer, zbuffer, backbuffer, camera);
	InitOIT(device, camera, oit_start, oit_scattered, oit_consolidated);
	
	InitVisualRender(vinfo, device, shadercache);
	InitLightRender(linfo, device, shadercache, camera);
	InitPostProcess(pinfo, device, shadercache);
	InitRayTracing(rinfo, device, shadercache);

	InitPhysics(physics);
	InitTiming(timing);
	InitInput(input);
	InitPlayer(player);
	InitScene(transforms, visuals, lights, geometries, physics, device);

	InitCBuffer(device, cb_object, sizeof( CBufferLayouts::object ));
	InitCBuffer(device, cb_object_z, sizeof( CBufferLayouts::object_z ));
	InitCBuffer(device, cb_object_cube_z, sizeof( CBufferLayouts::object_cube_z ));
	InitCBuffer(device, cb_light, sizeof( CBufferLayouts::light ));
	InitCBuffer(device, cb_frame, sizeof( CBufferLayouts::frame ));
	InitCBuffer(device, cb_tracy, sizeof( CBufferLayouts::tracy ));

	for (;;)
	{
		GetTiming(timing);
		GetInput(input);
		DerivePlayerState(player, input, timing);
		CrunchPhysics(physics, transforms, player, timing);
		DeriveCamera(transforms, player, camera);

		Prepare(graphics, cb_frame, zbuffer, oit_start, camera);
		RenderVisuals(graphics, vinfo, transforms, visuals, geometries,
			camera, oit_start, oit_scattered, oit_consolidated,
			gbuffer0, gbuffer1, zbuffer, cb_object, cb_frame);
		//RayTrace(graphics, rinfo, camera, zbuffer, gbuffer0, gbuffer1, cb_frame, cb_tracy);
		RenderLights(graphics, linfo, transforms, lights, visuals, geometries,
			camera, zbuffer, shadowmap, shadowcube, gbuffer0, gbuffer1, lbuffer,
			oit_start, oit_consolidated,
			cb_frame, cb_object_z, cb_object_cube_z, cb_light);
		PostProcess(graphics, pinfo, zbuffer, gbuffer0, gbuffer1, lbuffer, backbuffer, cb_frame);
		Present(device);
	}
}


} // namespace DEV

int main()
{
	try
	{
		DEV::run();
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