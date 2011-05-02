#ifndef __Devora_VisualRenderInfo__
#define __Devora_VisualRenderInfo__

#include "Matrix.h"
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
	Matrix4f proj;
	IPtr<ID3D11InputLayout> layout;
	std::vector<Geometry> geoms;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

} // namespace Devora

#endif