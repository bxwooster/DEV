#pragma once
#include "IPtr.hpp"
#include <D3D11.h>

namespace DEV {

struct PostProcessInfo
{
	IPtr<ID3D11VertexShader> vs_noop;
	IPtr<ID3D11GeometryShader> gs_fullscreen;
	IPtr<ID3D11PixelShader> ps_final;
	IPtr<ID3D11RasterizerState> rs_default;
	IPtr<ID3D11SamplerState> sm_point;
};

} // namespace DEV
