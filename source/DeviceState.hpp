#ifndef __Devora_DeviceState__
#define __Devora_DeviceState__

#include "IPtr.h"
#include <d3d11.h>

namespace Devora {

struct DeviceState
{
	IPtr<ID3D11Device> device;
	IPtr<IDXGISwapChain> swap_chain;
	HWND window;
};

} // namespace Devora

#endif