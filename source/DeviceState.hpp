#pragma once

#include "IPtr.h"
#include <d3d11.h>

namespace DEV {

struct DeviceState
{
	IPtr<ID3D11Device> device;
	IPtr<IDXGISwapChain> swap_chain;
};

} // namespace DEV
