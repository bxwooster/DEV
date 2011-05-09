#ifndef __Devora_VisualRenderInfo__
#define __Devora_VisualRenderInfo__

#include "IPtr.h"
#include <D3D11.h>

namespace Devora {

struct VisualRenderInfo
{
	IPtr<ID3D11InputLayout> layout;
	IPtr<ID3D11RasterizerState> rs_default;
	IPtr<ID3D11VertexShader> vs_render;
	IPtr<ID3D11PixelShader> ps_render;

	IPtr<ID3D11VertexShader> vs_noop;
	IPtr<ID3D11GeometryShader> gs_infinite_plane;
};

} // namespace Devora

#endif