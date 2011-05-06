#define NOMINMAX

#include "Buffer.hpp"
#include "Camera.hpp"
#include "DeviceState.hpp"
#include "GraphicsState.hpp"
#include "InputData.hpp"
#include "LightRenderInfo.hpp"
#include "PostProcessInfo.hpp"
#include "Lights.hpp"
#include "PhysicsState.hpp"
#include "PlayerState.hpp"
#include "TimingData.hpp"
#include "Transforms.hpp"
#include "VisualRenderInfo.hpp"
#include "Visuals.hpp"
#include "ZBuffer.hpp"
#include "CBuffer.hpp"

#include <exception>
#include <Windows.h>

namespace Devora {

void InitGraphics(GraphicsState& state, DeviceState& device,
	VisualRenderInfo& vinfo, LightRenderInfo& linfo, PostProcessInfo& pinfo,
	Buffer& gbuffer0, Buffer& gbuffer1, ZBuffer& shadowmap, ZBuffer& shadowcube,
	Buffer& lbuffer, ZBuffer& zbuffer, Buffer& backbuffer, Camera& camera,
	CBuffer& cb_object, CBuffer& cb_object_z, CBuffer& cb_object_cube_z,
	CBuffer& cb_light, CBuffer& cb_frame);

void InitInput(InputData& input);
void GetInput(InputData& input);

void InitTiming(TimingData& timing);
void GetTiming(TimingData& timing);

void InitScene(Transforms& transforms, Visuals& visuals, Lights& lights, PhysicsState& state);

void InitPhysics(PhysicsState& state);
void CrunchPhysics(PhysicsState& state, Transforms& transforms,
	PlayerState& player, TimingData& timing);

void DeriveCamera(Transforms& transforms, PlayerState& player, 
	DeviceState& device, Camera& camera, GraphicsState& state);
void DerivePlayerState(PlayerState& state, InputData& input, TimingData& timing);

void Present(DeviceState& state);

void RenderVisuals(GraphicsState& state, VisualRenderInfo& info, 
	Transforms& transforms, Visuals& visuals, Camera& camera,
	Buffer& gbuffer0, Buffer& gbuffer1, ZBuffer& zbuffer, CBuffer& cb_object);

void RenderLights(GraphicsState& state, VisualRenderInfo& vinfo, LightRenderInfo& info,
	Transforms& transforms, Lights& lights, Visuals& casters, Camera& camera,
	ZBuffer& zbuffer, ZBuffer& shadowmap, ZBuffer& shadowcube,
	Buffer& gbuffer0, Buffer& gbuffer1, Buffer& lbuffer);

void PostProcess(GraphicsState& state, ZBuffer& zbuffer, 
	Buffer& gbuffer0, Buffer& gbuffer1,	Buffer& lbuffer, Buffer& backbuffer);

void run()
{
	// Data
	GraphicsState graphics;
	DeviceState device;
	PhysicsState physics;

	VisualRenderInfo vinfo;
	LightRenderInfo linfo;
	PostProcessInfo pinfo;

	TimingData timing;
	InputData input;
	Camera camera;
	PlayerState player;

	Buffer backbuffer, gbuffer0, gbuffer1, lbuffer;
	ZBuffer zbuffer, shadowmap, shadowcube;

	Transforms transforms;
	Visuals visuals;
	Lights lights;

	CBuffer cb_object, cb_object_z, cb_object_cube_z, cb_light, cb_frame;

	// Code
	InitGraphics(graphics, device, vinfo, linfo, pinfo, gbuffer0, gbuffer1,
		shadowmap, shadowcube, lbuffer, zbuffer, backbuffer, camera,
		cb_object, cb_object_z, cb_object_cube_z, cb_light, cb_frame);
	InitPhysics(physics);
	InitTiming(timing);
	InitInput(input);
	InitScene(transforms, visuals, lights, physics);

	for (;;)
	{
		GetTiming(timing);
		GetInput(input);
		DerivePlayerState(player, input, timing);
		CrunchPhysics(physics, transforms, player, timing);
		DeriveCamera(transforms, player, device, camera, graphics); //!

		RenderVisuals(graphics, vinfo, transforms, visuals,
			camera, gbuffer0, gbuffer1, zbuffer, cb_object);
		RenderLights(graphics, vinfo, linfo, transforms, lights, visuals,
			camera, zbuffer, shadowmap, shadowcube, gbuffer0, gbuffer1, lbuffer);
		PostProcess(graphics, zbuffer, gbuffer0, gbuffer1, lbuffer, backbuffer);
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