#ifndef __Devora_Geometries__
#define __Devora_Geometries__

#include "IPtr.h"

#include <D3D11.h>
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

typedef std::vector<Geometry> Geometries;

} // namespace Devora

#endif