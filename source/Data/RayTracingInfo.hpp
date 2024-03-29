#pragma once
#include "IPtr.hpp"
#include <D3D11.h>

namespace DEV {

struct RayTracingInfo
{
	IPtr<ID3D11RasterizerState> rs_default;
	IPtr<ID3D11VertexShader> vs_noop;
	IPtr<ID3D11GeometryShader> gs_fullscreen;
	IPtr<ID3D11PixelShader> ps_tracy;
};

} // namespace DEV
