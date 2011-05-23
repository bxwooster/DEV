#pragma once
#include "IPtr.hpp"
#include <D3D11.h>
#include <vector>

namespace DEV {

struct Geometry
{
	IPtr<ID3D11Buffer> buffers[3]; //position, normal, index
	unsigned int count;
	unsigned int strides[2];
	unsigned int offsets[2];
};

typedef std::vector<Geometry> Geometries;

} // namespace DEV
