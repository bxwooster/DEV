#ifndef __Devora_LightRenderInfo__
#define __Devora_LightRenderInfo__

#include "Matrix.h"

namespace Devora {

struct LightRenderInfo
{
	Matrix4f proj;
	Matrix4f cubematrices[6];

	IPtr<ID3D11InputLayout> layout;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

} // namespace Devora

#endif