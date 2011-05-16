#include "OK.h"

#include "Tools.hpp"
#include "DeviceState.hpp"
#include "VisualRenderInfo.hpp"
#include "ShaderCache.hpp"
#include "LoadShader.hpp"

namespace DEV {

void InitVisualRender(VisualRenderInfo& info, DeviceState& device, ShaderCache& cache)
{
	LoadShader::Vertex(cache, device, info.vs_render, "shaders/vs_render.hlsl");
	LoadShader::Pixel(cache, device, info.ps_render, "shaders/ps_render.hlsl");

	LoadShader::Vertex(cache, device, info.vs_noop, "shaders/vs_noop.hlsl");
	LoadShader::Geometry(cache, device, info.gs_infinite_plane, "shaders/gs_infinite_plane.hlsl");

	LoadShader::Compute(cache, device, info.cs_oit_consolidate, "shaders/cs_oit_consolidate.hlsl");

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