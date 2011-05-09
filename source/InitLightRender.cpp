#include "Tools.hpp"
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

	{
		D3D11_SAMPLER_DESC desc = Tools::DefaultSamplerDesc();
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		HOK( device.device->CreateSamplerState( &desc, ~info.sm_point));
	}

	{
		D3D11_RASTERIZER_DESC desc = Tools::DefaultRasterizerDesc();
		desc.FrontCounterClockwise = true;
		HOK( device.device->CreateRasterizerState( &desc, ~info.rs_default));

		desc.CullMode = D3D11_CULL_NONE;
		HOK( device.device->CreateRasterizerState( &desc, ~info.rs_both_sides));

		desc.CullMode = D3D11_CULL_BACK;
		desc.SlopeScaledDepthBias = 1.0f;
		HOK( device.device->CreateRasterizerState( &desc, ~info.rs_shadow));
	}

	{
		D3D11_BLEND_DESC desc = Tools::DefaultBlendDesc();
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		HOK( device.device->CreateBlendState( &desc, ~info.bs_additive));
	}
}

} // namespace Devora