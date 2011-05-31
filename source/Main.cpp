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

#include "Tasking.hpp"
#include <exception>
#include <Windows.h>

namespace DEV {

struct Loop
{
	InOut(GraphicsState) graphics;
	InOut(DeviceState) device;
	InOut(PhysicsState) physics;
	InOut(ShaderCache) shadercache;

	InOut(VisualRenderInfo) vinfo;
	InOut(LightRenderInfo) linfo;
	InOut(PostProcessInfo) pinfo;
	InOut(RayTracingInfo) rinfo;

	InOut(TimingData) timing;
	InOut(InputData) input;
	InOut(Camera) camera;
	InOut(PlayerState) player;

	InOut(Buffer) backbuffer;
	InOut(Buffer) gbuffer;
	InOut(Buffer) lbuffer;
	InOut(ZBuffer) zbuffer;
	InOut(ZBuffer) shadowmap;
	InOut(ZBuffer) shadowcube;
	InOut(UBuffer) oit_start;
	InOut(UBuffer) oit_scattered;
	InOut(UBuffer) oit_consolidated;

	InOut(Transforms) transforms;
	InOut(Visuals) visuals;
	InOut(Lights) lights;
	InOut(Geometries) geometries;

	InOut(CBuffer) cb_object;
	InOut(CBuffer) cb_object_z;
	InOut(CBuffer) cb_object_cube_z;
	InOut(CBuffer) cb_light;
	InOut(CBuffer) cb_frame;
	InOut(CBuffer) cb_tracy;

	__End__;

	void run()
	{
		ScheduleTask( GetTiming,
			timing );

		ScheduleTask( GetInput,
			input );

		ScheduleTask( DerivePlayerState,
			player, input, timing );

		ScheduleTask( CrunchPhysics,
			physics, transforms, player, timing );

		ScheduleTask( DeriveCamera,
			transforms, player, camera );

		ScheduleTask( Prepare,
			graphics, cb_frame, zbuffer, oit_start, camera );
		ScheduleTask( RenderVisuals,
			graphics, vinfo, transforms, visuals, geometries,
			camera, oit_start, oit_scattered, oit_consolidated,
			gbuffer, zbuffer, cb_object, cb_frame );

		if (0) ScheduleTask( RayTrace,
		graphics, rinfo, camera, zbuffer, gbuffer, cb_frame, cb_tracy );

		ScheduleTask( RenderLights,
			graphics, linfo, transforms, lights, visuals, geometries,
			camera, zbuffer, shadowmap, shadowcube, gbuffer, lbuffer,
			oit_start, oit_scattered, oit_consolidated,
			cb_frame, cb_object_z, cb_object_cube_z, cb_light );

		ScheduleTask( PostProcess,
			graphics, pinfo, zbuffer, gbuffer, lbuffer, backbuffer, cb_frame );

		ScheduleTask( Present,
			device, graphics );

		ScheduleTaskObject( Loop, *this );
	}
};

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

	Loop loop = {
		graphics, device, physics, shadercache,
		vinfo, linfo, pinfo, rinfo,
		timing, input, camera, player,
		backbuffer, gbuffer, lbuffer,
		zbuffer, shadowmap, shadowcube,
		oit_start, oit_scattered, oit_consolidated,
		transforms, visuals, lights, geometries,
		cb_object, cb_object_z, cb_object_cube_z,
		cb_light, cb_frame, cb_tracy };

	Tasking::run_taskmanager(loop);
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