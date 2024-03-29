#pragma once
#include "IPtr.hpp"
#include <d3d11.h>

namespace DEV {

struct DeviceState
{
	IPtr<ID3D11Device> device;
	IPtr<IDXGISwapChain> swap_chain;
};

} // namespace DEV
