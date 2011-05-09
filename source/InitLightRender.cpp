#include "LightRenderInfo.hpp"
#include "DeviceState.hpp"
#include "ShaderCache.hpp"
#include "OK.h"

namespace Devora {
namespace LoadShader
{
	void Vertex(ShaderCache& cache, DeviceState& device, char* name, IPtr<ID3D11VertexShader>& shader);
	void Geometry(ShaderCache& cache, DeviceState& device, char* name, IPtr<ID3D11GeometryShader>& shader);
	void Pixel(ShaderCache& cache, DeviceState& device, char* name, IPtr<ID3D11PixelShader>& shader);
}

void InitLightRender(DeviceState& device, ShaderCache& cache, LightRenderInfo& info)
{
	LoadShader::Vertex(cache, device, "shaders/vs_render_z.hlsl", info.vs_render_z);
	LoadShader::Vertex(cache, device, "shaders/vs_render_cube_z.hlsl", info.vs_render_cube_z);
	LoadShader::Vertex(cache, device, "shaders/vs_noop.hlsl", info.vs_noop);
	LoadShader::Geometry(cache, device, "shaders/gs_fullscreen.hlsl", info.gs_fullscreen);
	LoadShader::Geometry(cache, device, "shaders/gs_render_cube_z.hlsl", info.gs_render_cube_z);
	LoadShader::Geometry(cache, device, "shaders/gs_dir_light.hlsl", info.gs_dir_light);
	LoadShader::Geometry(cache, device, "shaders/gs_point_light.hlsl", info.gs_point_light);
	LoadShader::Pixel(cache, device, "shaders/ps_dir_light.hlsl", info.ps_dir_light);
	LoadShader::Pixel(cache, device, "shaders/ps_point_light.hlsl", info.ps_point_light);
}

} // namespace Devora