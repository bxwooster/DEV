#ifndef __Devora_VisualRenderInfo__
#define __Devora_VisualRenderInfo__

#include <vector>

namespace Devora {

struct Geometry
{
	typedef unsigned int uint;
	IPtr<ID3D11Buffer> buffer;
	uint stride;
	uint offset;
	uint count;
};

struct VisualRenderInfo
{
	IPtr<ID3D11InputLayout> layout;
	IPtr<ID3D11RasterizerState> rasterizerstate;
	IPtr<ID3D11VertexShader> vshader;
	IPtr<ID3D11PixelShader> pshader;

	std::vector<Geometry> geoms;
};

} // namespace Devora

#endif