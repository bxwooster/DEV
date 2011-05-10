#include "OK.h"

#include "Tools.hpp"
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

void InitVisualRender(VisualRenderInfo& info, DeviceState& device, ShaderCache& cache)
{
	LoadShader::Vertex(cache, device, "shaders/vs_render.hlsl", info.vs_render);
	LoadShader::Pixel(cache, device, "shaders/ps_render.hlsl", info.ps_render);

	LoadShader::Vertex(cache, device, "shaders/vs_noop.hlsl", info.vs_noop);
	LoadShader::Geometry(cache, device, "shaders/gs_infinite_plane.hlsl", info.gs_infinite_plane);

	{
		D3D11_RASTERIZER_DESC desc = Tools::DefaultRasterizerDesc();
		//desc.FrontCounterClockwise = true;
		HOK( device.device->CreateRasterizerState( &desc, ~info.rs_default));
	}

	// Layout //!
	{       
		IPtr<ID3D10Blob> code;
		Tools::CompileShader( "shaders/vs_render.hlsl", "vs_5_0", ~code );

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

} // namespace Devora