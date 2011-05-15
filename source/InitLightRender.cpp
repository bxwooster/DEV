#include "Tools.hpp"
#include "Camera.hpp"
#include "LightRenderInfo.hpp"
#include "DeviceState.hpp"
#include "ShaderCache.hpp"
#include "OK.h"

namespace DEV {
namespace LoadShader
{
	void Vertex(ShaderCache& cache, DeviceState& device, IPtr<ID3D11VertexShader>& shader, char* name, char* entry = "main");
	void Geometry(ShaderCache& cache, DeviceState& device, IPtr<ID3D11GeometryShader>& shader, char* name, char* entry = "main");
	void Pixel(ShaderCache& cache, DeviceState& device, IPtr<ID3D11PixelShader>& shader, char* name, char* entry = "main");
}

void InitLightRender(LightRenderInfo& info, DeviceState& device, ShaderCache& cache, Camera& camera)
{
	LoadShader::Vertex(cache, device, info.vs_render_z, "shaders/vs_render_z.hlsl");
	LoadShader::Vertex(cache, device, info.vs_render_cube_z, "shaders/vs_render_cube_z.hlsl");
	LoadShader::Vertex(cache, device, info.vs_noop, "shaders/vs_noop.hlsl");
	LoadShader::Geometry(cache, device, info.gs_fullscreen, "shaders/gs_fullscreen.hlsl");
	LoadShader::Geometry(cache, device, info.gs_render_cube_z, "shaders/gs_render_cube_z.hlsl");
	LoadShader::Geometry(cache, device, info.gs_dir_light, "shaders/gs_dir_light.hlsl");
	LoadShader::Geometry(cache, device, info.gs_point_light, "shaders/gs_point_light.hlsl");
	LoadShader::Pixel(cache, device, info.ps_dir_light, "shaders/ps_dir_light.hlsl");
	LoadShader::Pixel(cache, device, info.ps_point_light, "shaders/ps_point_light.hlsl");
	LoadShader::Pixel(cache, device, info.ps_dir_light_oit, "shaders/ps_dir_light.hlsl", "main_oit");
	LoadShader::Pixel(cache, device, info.ps_point_light_oit, "shaders/ps_point_light.hlsl", "main_oit");

	{
		D3D11_SAMPLER_DESC desc = Tools::DefaultSamplerDesc();
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		HOK( device.device->CreateSamplerState( &desc, ~info.sm_point));
	}

	{
		D3D11_RASTERIZER_DESC desc = Tools::DefaultRasterizerDesc();
		HOK( device.device->CreateRasterizerState( &desc, ~info.rs_default));

		desc.CullMode = D3D11_CULL_FRONT;
		HOK( device.device->CreateRasterizerState( &desc, ~info.rs_backface));

		desc.CullMode = D3D11_CULL_BACK;
		desc.SlopeScaledDepthBias = 1.0f;
		desc.DepthBias = 128;
		HOK( device.device->CreateRasterizerState( &desc, ~info.rs_shadow));
	}

	{
		D3D11_BLEND_DESC desc = Tools::DefaultBlendDesc();
		desc.RenderTarget[0].BlendEnable = true;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		HOK( device.device->CreateBlendState( &desc, ~info.bs_additive));
	}

	// Layout //!
	{       
		IPtr<ID3D10Blob> code;

		D3D11_INPUT_ELEMENT_DESC element[2] =
		{
			{
				"POSITION", 0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0, D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA, 0
			},

			{
				"NORMAL", 0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				1, D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA, 0
			}
		};

		Tools::CompileShader( "shaders/vs_render.hlsl", "main", "vs_5_0", ~code );
		HOK( device.device->CreateInputLayout
			( element, 2, code->GetBufferPointer(),
			code->GetBufferSize(), ~info.layout ) );

		Tools::CompileShader( "shaders/vs_render_z.hlsl", "main", "vs_5_0", ~code );
		HOK( device.device->CreateInputLayout
			( element, 1, code->GetBufferPointer(),
			code->GetBufferSize(), ~info.layout_z ) );
	}

	//Misc
	{
		info.cubematrices[0] <<
				0, 0, 1, 0,
				0, 1, 0, 0,
				-1, 0, 0, 0,
				0, 0, 0, 1;
		info.cubematrices[1] <<
				0, 0,-1, 0,
				0, 1, 0, 0,
				1, 0, 0, 0,
				0, 0, 0, 1;
		info.cubematrices[2] <<
				1, 0, 0, 0,
				0, 0, 1, 0,
				0,-1, 0, 0,
				0, 0, 0, 1;
		info.cubematrices[3] <<
				1, 0, 0, 0,
				0, 0,-1, 0,
				0, 1, 0, 0,
				0, 0, 0, 1;
		info.cubematrices[4] <<
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1;
		info.cubematrices[5] <<
				-1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0,-1, 0,
				0, 0, 0, 1;

		Tools::SetProjectionMatrix(info.proj, 90, 1.0, camera.z_near); //!
	}
}

} // namespace DEV