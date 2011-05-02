#define NOMINMAX

#include "Buffer.hpp"
#include "Camera.hpp"
#include "GraphicsState.hpp"
#include "InputData.hpp"
#include "LightRenderInfo.hpp"
#include "Lights.hpp"
#include "PhysicsState.hpp"
#include "PlayerState.hpp"
#include "PostProcessInfo.hpp"
#include "TimingData.hpp"
#include "Transforms.hpp"
#include "VisualRenderInfo.hpp"
#include "Visuals.hpp"
#include "ZBuffer.hpp"

#include <exception>
#include <Windows.h>

namespace Devora {

void InitGraphics(GraphicsState& state, 
	VisualRenderInfo& vinfo, LightRenderInfo& linfo, PostProcessInfo& pinfo, 
	Buffer& gbuffer0, Buffer& gbuffer1, ZBuffer& shadowmap, ZBuffer& shadowcube,
	Buffer& lbuffer, ZBuffer& zbuffer, Buffer& backbuffer);

void InitInput(InputData& input);
void GetInput(InputData& input);

void InitTiming(TimingData& timing);
void GetTiming(TimingData& timing);

void InitScene(Transforms& transforms, Visuals& visuals, Lights& lights, PhysicsState& state);

void InitPhysics(PhysicsState& state);
void CrunchPhysics(PhysicsState& state, Transforms& transforms,
	PlayerState& player, TimingData& timing);

void DeriveCamera(Transforms& transforms, PlayerState& player, Camera& camera);
void DerivePlayerState(PlayerState& state, InputData& input, TimingData& timing);

void Present(GraphicsState& state);

void RenderVisuals(GraphicsState& state, VisualRenderInfo& info, 
	Transforms& transforms, Visuals& visuals, Camera& camera,
	Buffer& gbuffer0, Buffer& gbuffer1, ZBuffer& zbuffer);

void RenderLights(GraphicsState& state, VisualRenderInfo& vinfo, LightRenderInfo& info,
	Transforms& transforms, Lights& lights, Visuals& casters, Camera& camera,
	ZBuffer& zbuffer, ZBuffer& shadowmap, ZBuffer& shadowcube,
	Buffer& gbuffer0, Buffer& gbuffer1, Buffer& lbuffer);

void PostProcess(GraphicsState& state, PostProcessInfo& info, ZBuffer& zbuffer, 
	Buffer& gbuffer0, Buffer& gbuffer1,	Buffer& lbuffer, Buffer& backbuffer, Camera& camera);

void run()
{
	GraphicsState graphics;
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

	InitGraphics(graphics, vinfo, linfo, pinfo, gbuffer0, gbuffer1,
		shadowmap, shadowcube, lbuffer, zbuffer, backbuffer);
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
		DeriveCamera(transforms, player, camera);

		RenderVisuals(graphics, vinfo, transforms, visuals,
			camera, gbuffer0, gbuffer1, zbuffer);
		RenderLights(graphics, vinfo, linfo, transforms, lights, visuals,
			camera, zbuffer, shadowmap, shadowcube, gbuffer0, gbuffer1, lbuffer);
		PostProcess(graphics, pinfo, zbuffer, gbuffer0, gbuffer1, lbuffer, backbuffer, camera);
		Present(graphics);
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