#ifndef __Devora_PostProcessInfo__
#define __Devora_PostProcessInfo__

#include "Matrix.h"

namespace Devora {

struct PostProcessInfo
{
	IPtr<ID3D11VertexShader> vshader_fs;
	IPtr<ID3D11GeometryShader> gshader_fs;
	IPtr<ID3D11PixelShader> pshader;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

} // namespace Devora

#endif