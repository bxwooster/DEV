#ifndef __Devora_Geometries__
#define __Devora_Geometries__

#include "IPtr.h"
#include <D3D11.h>
#include <vector>

namespace Devora {

struct Geometry
{
	IPtr<ID3D11Buffer> buffers[2];
	unsigned int count;
	unsigned int strides[2];
	unsigned int offsets[2];
};

typedef std::vector<Geometry> Geometries;

} // namespace Devora

#endif