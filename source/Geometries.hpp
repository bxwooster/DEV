#ifndef __Devora_Geometries__
#define __Devora_Geometries__

#include "IPtr.h"
#include <D3D11.h>
#include <vector>

namespace Devora {

struct Geometry
{
	IPtr<ID3D11Buffer> buffer;
	unsigned int count;
	unsigned int stride;
	unsigned int offset;
};

typedef std::vector<Geometry> Geometries;

} // namespace Devora

#endif