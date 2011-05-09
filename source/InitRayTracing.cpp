#include "OK.h"

#include "DeviceState.hpp"
#include "RayTracingInfo.hpp"
#include "ShaderCache.hpp"

namespace Devora {
namespace LoadShader
{
	void Vertex(ShaderCache& cache, DeviceState& device, char* name, IPtr<ID3D11VertexShader>& shader);
	void Geometry(ShaderCache& cache, DeviceState& device, char* name, IPtr<ID3D11GeometryShader>& shader);
	void Pixel(ShaderCache& cache, DeviceState& device, char* name, IPtr<ID3D11PixelShader>& shader);
}

void InitRayTracing(DeviceState& device, ShaderCache& cache, RayTracingInfo& info)
{
	LoadShader::Vertex(cache, device, "shaders/vs_noop.hlsl", info.vs_noop);
	LoadShader::Geometry(cache, device, "shaders/gs_fullscreen.hlsl", info.gs_fullscreen);
	LoadShader::Pixel(cache, device, "shaders/ps_tracy.hlsl", info.ps_tracy);
}

} // namespace Devora