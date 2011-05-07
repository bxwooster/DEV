#ifndef __Devora_RayTraceInfo__
#define __Devora_RayTraceInfo__

#include "IPtr.h"
#include <D3D11.h>

namespace Devora {

struct RayTracingInfo
{
	IPtr<ID3D11RasterizerState> rs_default;
	IPtr<ID3D11VertexShader> vs_noop;
	IPtr<ID3D11GeometryShader> gs_fullscreen;
	IPtr<ID3D11PixelShader> ps_tracy;
};

} // namespace Devora

#endif