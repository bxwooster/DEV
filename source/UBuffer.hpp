#pragma once

#include "IPtr.h"
#include <D3D11.h>

namespace DEV {

struct UBuffer
{
	IPtr<ID3D11Buffer> buffer;
	IPtr<ID3D11UnorderedAccessView> uav;
};

} // namespace DEV
