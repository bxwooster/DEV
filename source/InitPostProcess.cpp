#include "OK.h"

#include "Tools.hpp"
#include "DeviceState.hpp"
#include "PostProcessInfo.hpp"
#include "ShaderCache.hpp"

namespace DEV {
namespace LoadShader
{
	void Vertex(ShaderCache& cache, DeviceState& device, IPtr<ID3D11VertexShader>& shader, char* name, char* entry = "main");
	void Geometry(ShaderCache& cache, DeviceState& device, IPtr<ID3D11GeometryShader>& shader, char* name, char* entry = "main");
	void Pixel(ShaderCache& cache, DeviceState& device, IPtr<ID3D11PixelShader>& shader, char* name, char* entry = "main");
}

void InitPostProcess(PostProcessInfo& info, DeviceState& device, ShaderCache& cache)
{
	LoadShader::Vertex(cache, device, info.vs_noop, "shaders/vs_noop.hlsl");
	LoadShader::Geometry(cache, device, info.gs_fullscreen, "shaders/gs_fullscreen.hlsl");
	LoadShader::Pixel(cache, device, info.ps_final, "shaders/ps_final.hlsl");

	{
		D3D11_SAMPLER_DESC desc = Tools::DefaultSamplerDesc();
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		HOK( device.device->CreateSamplerState( &desc, ~info.sm_point));
	}

	{
		D3D11_RASTERIZER_DESC desc = Tools::DefaultRasterizerDesc();
		//desc.FrontCounterClockwise = true;
		HOK( device.device->CreateRasterizerState( &desc, ~info.rs_default));
	}
}

} // namespace DEV