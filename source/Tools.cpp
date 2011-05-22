#include "Matrix.hpp"
#include "OK.hpp"

#include "Data/Geometries.hpp"
#include "Data/DeviceState.hpp"

#include <D3DX11.h>
#include <fstream>
#include <vector>

namespace DEV {

typedef unsigned int uint;

namespace Tools {

D3D11_BLEND_DESC DefaultBlendDesc()
{
	static D3D11_BLEND_DESC desc;
	static bool once = false;
	if (!once)
	{
		once = true;
		desc.AlphaToCoverageEnable = false;
		desc.IndependentBlendEnable = false;
		for (int i = 0; i < 8; ++i)
		{
			desc.RenderTarget[i].BlendEnable = false;
			desc.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
			desc.RenderTarget[i].DestBlend = D3D11_BLEND_ZERO;
			desc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
			desc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
			desc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		}
	}
	return desc;
}

D3D11_RASTERIZER_DESC DefaultRasterizerDesc()
{
	static D3D11_RASTERIZER_DESC desc;
	static bool once = false;
	if (!once)
	{
		once = true;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_BACK;
		desc.FrontCounterClockwise = false;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0.0f;
		desc.SlopeScaledDepthBias = 0.0f;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = false;
		desc.AntialiasedLineEnable = false;
	}
	return desc;
}

D3D11_SAMPLER_DESC DefaultSamplerDesc()
{
	static D3D11_SAMPLER_DESC desc;
	static bool once = false;
	if (!once)
	{
		once = true;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;
		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = 16;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		for (int i = 0; i < 4; ++i)
			desc.BorderColor[i] = 0.0f;
	}
	return desc;
}

D3D11_DEPTH_STENCIL_DESC DefaultDepthStencilDesc()
{
	static D3D11_DEPTH_STENCIL_DESC desc;
	static bool once = false;
	if (!once)
	{
		once = true;

		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS;
		desc.StencilEnable = false;
		desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				
		desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	}

	return desc;
}

void CompileShader( char* file, char* entry, char* profile, ID3D10Blob** code )
{
	IPtr<ID3D10Blob> info;

	unsigned int shader_flags = D3D10_SHADER_ENABLE_STRICTNESS |
	D3D10_SHADER_OPTIMIZATION_LEVEL0 |
	D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;

	HOK_EX( D3DX11CompileFromFile( file,
	NULL, NULL, entry, profile, shader_flags,
	0, NULL, code, ~info, NULL ),
	*&info ? (char*)info->GetBufferPointer() : "" );
}

void SetProjectionMatrix(Matrix4f& proj, float y_fov, float aspect_ratio, float z_near)
{
	float ys = 1 / tanf( y_fov * float(M_PI) / 180 * 0.5f );
	float xs = ys / aspect_ratio;
	proj << xs, 0, 0, 0,
			0, ys, 0, 0,
			0, 0, -1, -z_near,
			0, 0, -1, 0;
}

Geometry ReadGeometry(ID3D11Device* device, const std::string& path)
{
	using std::ifstream;

	Geometry geometry;
	uint width;

	for (int i = 0; i < 3; i++)
	{
		char* name = "position";
		if (i == 1) name = "normal";
		if (i == 2) name = "index";
		ifstream file( path + "\\" + name, ifstream::in | ifstream::binary | ifstream::ate );
		file.exceptions( ifstream::eofbit | ifstream::failbit | ifstream::badbit );

		width = uint(file.tellg());
    
		std::vector<char> data(width);
		file.seekg(0);
		file.read( &data[0], width ); 

		D3D11_SUBRESOURCE_DATA init;
		init.pSysMem = &data[0];

		D3D11_BUFFER_DESC buffer_desc;
		buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
		buffer_desc.BindFlags = i == 2 ? D3D11_BIND_INDEX_BUFFER : D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.ByteWidth = width;

		HOK( device->CreateBuffer
			( &buffer_desc, &init, ~geometry.buffers[i] ) );
	}

	geometry.strides[0] = 3 * sizeof(float);
	geometry.strides[1] = 3 * sizeof(float);
	geometry.offsets[0] = 0;
	geometry.offsets[1] = 0;
	geometry.count = width / sizeof(unsigned short);
	
	return geometry;
}

}

} // namespace DEV