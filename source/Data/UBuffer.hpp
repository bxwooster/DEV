#pragma once
#include "IPtr.hpp"
#include <D3D11.h>

namespace DEV {

struct UBuffer
{
	IPtr<ID3D11Buffer> buffer;
	IPtr<ID3D11UnorderedAccessView> uav;
};

} // namespace DEV
