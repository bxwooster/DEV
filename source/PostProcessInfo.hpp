#ifndef __Devora_PostProcessInfo__
#define __Devora_PostProcessInfo__

#include "Matrix.h"

namespace Devora {

struct PostProcessInfo
{
	IPtr<ID3D11VertexShader> vs_noop;
	IPtr<ID3D11GeometryShader> gs_fullscreen;
	IPtr<ID3D11PixelShader> ps_final;
	IPtr<ID3D11SamplerState> sm_point;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

} // namespace Devora

#endif