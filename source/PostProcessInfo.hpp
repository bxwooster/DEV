#ifndef __Devora_PostProcessInfo__
#define __Devora_PostProcessInfo__

#include "IPtr.h"
#include <D3D11.h>

namespace Devora {

struct PostProcessInfo
{
	IPtr<ID3D11VertexShader> vs_noop;
	IPtr<ID3D11GeometryShader> gs_fullscreen;
	IPtr<ID3D11PixelShader> ps_final;
	IPtr<ID3D11RasterizerState> rs_default;
	IPtr<ID3D11SamplerState> sm_point;
};

} // namespace Devora

#endif