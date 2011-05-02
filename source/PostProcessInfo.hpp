#ifndef __Devora_PostProcessInfo__
#define __Devora_PostProcessInfo__

#include "Matrix.h"

namespace Devora {

struct PostProcessInfo
{
	Vector3f ambient;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

} // namespace Devora

#endif