#include "OK.h"

#include "DeviceState.hpp"
#include "VisualRenderInfo.hpp"
#include "ShaderCache.hpp"

namespace Devora {
namespace LoadShader
{
	void Vertex(ShaderCache& cache, DeviceState& device, char* name, IPtr<ID3D11VertexShader>& shader);
	void Geometry(ShaderCache& cache, DeviceState& device, char* name, IPtr<ID3D11GeometryShader>& shader);
	void Pixel(ShaderCache& cache, DeviceState& device, char* name, IPtr<ID3D11PixelShader>& shader);
}

void InitVisualRender(DeviceState& device, ShaderCache& cache, VisualRenderInfo& info)
{
	LoadShader::Vertex(cache, device, "shaders/vs_render.hlsl", info.vs_render);
	LoadShader::Pixel(cache, device, "shaders/ps_render.hlsl", info.ps_render);
}

} // namespace Devora