#include "Matrix.h"
#include "ok.h"

#include "Geometries.hpp"
#include "DeviceState.hpp"

#include <D3DX11.h>
#include <fstream>

namespace Devora {

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

void CompileShader( char* file, char* profile, ID3D10Blob** code )
{
	IPtr<ID3D10Blob> info;

	unsigned int shader_flags = D3D10_SHADER_ENABLE_STRICTNESS |
	D3D10_SHADER_OPTIMIZATION_LEVEL0 |
	D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;

	HOK_EX( D3DX11CompileFromFile( file,
	NULL, NULL, "main", profile, shader_flags,
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

Geometry ReadGeometry(ID3D11Device* device, const std::string& filename)
{
	using std::ifstream;

	Geometry geometry;

	ifstream file( filename, ifstream::in | ifstream::binary );
	file.exceptions( ifstream::eofbit | ifstream::failbit | ifstream::badbit );

	char head[8];
	char ver[4];
    
	file.read( head, 8 );
	file.read( ver, 4 );

	uint elements;
	file.read( (char*)&elements, 4 );

	uint fmt, index, len;

	for ( uint i = 0; i < elements; i++ )
	{
		file.read( (char*)&fmt, 4 );
		file.read( (char*)&index, 4 );
		file.read( (char*)&len, 4 );
        
		char* semantic = new char[len + 1];
		file.read( (char*)semantic, len );
		delete[] semantic;
	}
    
	geometry.offset = 0;
	file.read( (char*)&geometry.stride, 4 );
    
	uint width;
	file.read( (char*)&width, 4 );
	geometry.count = width / geometry.stride;
    
	char* vertices = new char[width];
	file.read( vertices, width ); 

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = vertices;

	D3D11_BUFFER_DESC buffer_desc;
	buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.ByteWidth = width;

	HOK( device->CreateBuffer
		( &buffer_desc, &data, ~geometry.buffer ) );

	delete[] vertices;

	return geometry;
}

}

} // namespace Devora