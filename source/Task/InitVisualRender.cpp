#define NOMINMAX
#include "InitVisualRender.hpp"
#include "OK.hpp"
#include "Tools.hpp"
#include "LoadShader.hpp"

namespace DEV {

void InitVisualRender::run()
{
	LoadShader::Vertex(cache, device, info.vs_render, "shaders/vs_render.hlsl");
	LoadShader::Pixel(cache, device, info.ps_render, "shaders/ps_render.hlsl");

	LoadShader::Vertex(cache, device, info.vs_noop, "shaders/vs_noop.hlsl");
	LoadShader::Geometry(cache, device, info.gs_infinite_plane, "shaders/gs_infinite_plane.hlsl");

	{
		D3D11_SAMPLER_DESC desc = Tools::DefaultSamplerDesc();
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		HOK( device.device->CreateSamplerState( &desc, ~info.sm_point));
	}

	{
		D3D11_RASTERIZER_DESC desc = Tools::DefaultRasterizerDesc();
		HOK( device.device->CreateRasterizerState( &desc, ~info.rs_default));
	}

	// Layout //!
	{       
		IPtr<ID3D10Blob> code;
		Tools::CompileShader( "shaders/vs_render.hlsl", "main", "vs_5_0", ~code );

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

		HOK( device.device->CreateInputLayout
			( element, 2, code->GetBufferPointer(),
			code->GetBufferSize(), ~info.layout ) );
	}
}

} // namespace DEV