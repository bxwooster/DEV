#define NOMINMAX

#include "Task/InitOIT.hpp"
#include "Task/InitGraphics.hpp"
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

#include "Task.hpp"
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
	RunTask( InitGraphics,
		graphics, device, gbuffer, shadowmap, shadowcube,
		lbuffer, zbuffer, backbuffer, camera,
		cb_object, cb_object_z, cb_object_cube_z,
		cb_light, cb_frame, cb_tracy );

	RunTask( InitOIT,
		device, camera, oit_start, oit_scattered, oit_consolidated );
	
	RunTask( InitVisualRender, 
		vinfo, device, shadercache );

	RunTask( InitLightRender,
		linfo, device, shadercache, camera );

	RunTask( InitPostProcess,
		pinfo, device, shadercache );

	RunTask( InitRayTracing,
		rinfo, device, shadercache );

	RunTask( InitPhysics,
		physics );

	RunTask( InitTiming,
		timing );

	RunTask( InitInput,
		input );

	RunTask( InitPlayer,
		player );

	RunTask( InitScene,
		transforms, visuals, lights, geometries, physics, device );

	for (;;)
	{
		RunTask( GetTiming,
			timing );

		RunTask( GetInput,
			input );

		RunTask( DerivePlayerState,
			player, input, timing );

		RunTask( CrunchPhysics,
			physics, transforms, player, timing );

		RunTask( DeriveCamera,
			transforms, player, camera );

		RunTask( Prepare,
			graphics, cb_frame, zbuffer, oit_start, camera );

		RunTask( RenderVisuals,
			graphics, vinfo, transforms, visuals, geometries,
			camera, oit_start, oit_scattered, oit_consolidated,
			gbuffer, zbuffer, cb_object, cb_frame );

		if (0) RunTask( RayTrace,
		graphics, rinfo, camera, zbuffer, gbuffer, cb_frame, cb_tracy );

		RunTask( RenderLights,
			graphics, linfo, transforms, lights, visuals, geometries,
			camera, zbuffer, shadowmap, shadowcube, gbuffer, lbuffer,
			oit_start, oit_scattered, oit_consolidated,
			cb_frame, cb_object_z, cb_object_cube_z, cb_light );

		RunTask( PostProcess,
			graphics, pinfo, zbuffer, gbuffer, lbuffer, backbuffer, cb_frame );

		RunTask( Present,
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