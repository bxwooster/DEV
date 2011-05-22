#define NOMINMAX
#include "Data/DeviceState.hpp"
#include "Data/GraphicsState.hpp"
#include "Data/InputData.hpp"
#include "Data/TimingData.hpp"
#include "Data/LightRenderInfo.hpp"
#include "Data/PostProcessInfo.hpp"
#include "Data/RayTracingInfo.hpp"
#include "Data/VisualRenderInfo.hpp"
#include "Data/PhysicsState.hpp"
#include "Data/PlayerState.hpp"
#include "Data/Camera.hpp"
#include "Data/Transforms.hpp"
#include "Data/Visuals.hpp"
#include "Data/Lights.hpp"
#include "Data/Geometries.hpp"
#include "Data/Buffer.hpp"
#include "Data/ZBuffer.hpp"
#include "Data/UBuffer.hpp"
#include "Data/CBuffer.hpp"
#include "Data/ShaderCache.hpp"

#include "Task/InitOIT.hpp"
#include "Task/InitGraphics.hpp"
#include "Task/InitCBuffer.hpp"
#include "Task/InitVisualRender.hpp"
#include "Task/InitLightRender.hpp"
#include "Task/InitPostProcess.hpp"
#include "Task/InitRayTracing.hpp"
#include "Task/InitPlayer.hpp"
#include "Task/InitInput.hpp"
#include "Task/GetInput.hpp"
#include "Task/InitTiming.hpp"
#include "Task/InitScene.hpp"
#include "Task/InitPhysics.hpp"
#include "Task/DerivePlayerState.hpp"
#include "Task/DeriveCamera.hpp"
#include "Task/Present.hpp"
#include "Task/Prepare.hpp"
#include "Task/RenderVisuals.hpp"
#include "Task/RenderLights.hpp"
#include "Task/PostProcess.hpp"
#include "Task/RayTrace.hpp"
#include "Task/CrunchPhysics.hpp"
#include "Task/GetInput.hpp"
#include "Task/GetTiming.hpp"

#include "CBufferLayouts.hpp"
#include "TaskManager.hpp"
#include <exception>
#include <Windows.h>

namespace DEV {

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

	Buffer backbuffer, gbuffer, lbuffer;
	ZBuffer zbuffer, shadowmap, shadowcube;
	UBuffer oit_start, oit_scattered, oit_consolidated;

	Transforms transforms;
	Visuals visuals;
	Lights lights;
	Geometries geometries;

	CBuffer cb_object, cb_object_z, cb_object_cube_z, cb_light, cb_frame, cb_tracy;

	// Code
	Run( InitGraphics,
		graphics, device, gbuffer, shadowmap, shadowcube,
		lbuffer, zbuffer, backbuffer, camera );

	Run( InitOIT,
		device, camera, oit_start, oit_scattered, oit_consolidated );
	
	Run( InitVisualRender, 
		vinfo, device, shadercache );

	Run( InitLightRender,
		linfo, device, shadercache, camera );

	Run( InitPostProcess,
		pinfo, device, shadercache );

	Run( InitRayTracing,
		rinfo, device, shadercache );

	Run( InitPhysics,
		physics );

	Run( InitTiming,
		timing );

	Run( InitInput,
		input );

	Run( InitPlayer,
		player );

	Run( InitScene,
		transforms, visuals, lights, geometries, physics, device );

	Run( InitCBuffer,
		device, cb_object, sizeof( CBufferLayouts::object ) );

	Run( InitCBuffer,
		device, cb_object_z, sizeof( CBufferLayouts::object_z ) );

	Run( InitCBuffer,
		device, cb_object_cube_z, sizeof( CBufferLayouts::object_cube_z ) );

	Run( InitCBuffer,
		device, cb_light, sizeof( CBufferLayouts::light ) );

	Run( InitCBuffer,
		device, cb_frame, sizeof( CBufferLayouts::frame ) );

	Run( InitCBuffer,
		device, cb_tracy, sizeof( CBufferLayouts::tracy ) );

	for (;;)
	{
		Run( GetTiming,
			timing );

		Run( GetInput,
			input );

		Run( DerivePlayerState,
			player, input, timing );

		Run( CrunchPhysics,
			physics, transforms, player, timing );

		Run( DeriveCamera,
			transforms, player, camera );

		Run( Prepare,
			graphics, cb_frame, zbuffer, oit_start, camera );

		Run( RenderVisuals,
			graphics, vinfo, transforms, visuals, geometries,
			camera, oit_start, oit_scattered, oit_consolidated,
			gbuffer, zbuffer, cb_object, cb_frame );

		Run( RayTrace,
		graphics, rinfo, camera, zbuffer, gbuffer, cb_frame, cb_tracy );

		Run( RenderLights,
			graphics, linfo, transforms, lights, visuals, geometries,
			camera, zbuffer, shadowmap, shadowcube, gbuffer, lbuffer,
			oit_start, oit_scattered, oit_consolidated,
			cb_frame, cb_object_z, cb_object_cube_z, cb_light );

		Run( PostProcess,
			graphics, pinfo, zbuffer, gbuffer, lbuffer, backbuffer, cb_frame );

		Run( Present,
			device );
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