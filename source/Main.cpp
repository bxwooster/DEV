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

		ScheduleTaskObject( Loop, *this );
	}
};

struct Init
{
	__End__;

	void run()
	{
		// Data
		auto& graphics = *new GraphicsState;
		auto& device = *new DeviceState;
		auto& physics = *new PhysicsState;
		auto& shadercache = *new ShaderCache;

		auto& vinfo = *new VisualRenderInfo;
		auto& linfo = *new LightRenderInfo;
		auto& pinfo = *new PostProcessInfo;
		auto& rinfo = *new RayTracingInfo;

		auto& timing = *new TimingData;
		auto& input = *new InputData;
		auto& camera = *new Camera;
		auto& player = *new PlayerState;

		auto& backbuffer = *new Buffer;
		auto& gbuffer = *new Buffer;
		auto& lbuffer = *new Buffer;
		auto& zbuffer = *new ZBuffer;
		auto& shadowmap = *new ZBuffer;
		auto& shadowcube = *new ZBuffer;
		auto& oit_start = *new UBuffer;
		auto& oit_scattered = *new UBuffer;
		auto& oit_consolidated = *new UBuffer;

		auto& transforms = *new Transforms;
		auto& visuals = *new Visuals;
		auto& lights = *new Lights;
		auto& geometries = *new Geometries;

		auto& cb_object = *new CBuffer;
		auto& cb_object_z = *new CBuffer;
		auto& cb_object_cube_z = *new CBuffer;
		auto& cb_light = *new CBuffer;
		auto& cb_frame = *new CBuffer;
		auto& cb_tracy = *new CBuffer;

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

		ScheduleTask( Loop,
			graphics, device, physics, shadercache,
			vinfo, linfo, pinfo, rinfo,
			timing, input, camera, player,
			backbuffer, gbuffer, lbuffer,
			zbuffer, shadowmap, shadowcube,
			oit_start, oit_scattered, oit_consolidated,
			transforms, visuals, lights, geometries,
			cb_object, cb_object_z, cb_object_cube_z,
			cb_light, cb_frame, cb_tracy );
	}
};

} // namespace DEV

int main()
{
	try
	{
		DEV::Init init = {};
		Tasking::run_taskmanager(init);
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