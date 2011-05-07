#ifndef __Devora_VisualRenderInfo__
#define __Devora_VisualRenderInfo__

#include <vector>

namespace Devora {

struct VisualRenderInfo
{
	IPtr<ID3D11InputLayout> layout;
	IPtr<ID3D11RasterizerState> rs_default;
	IPtr<ID3D11VertexShader> vs_render;
	IPtr<ID3D11PixelShader> ps_render;
};

} // namespace Devora

#endif