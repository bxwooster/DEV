#include "OK.h"

#include "DeviceState.hpp"
#include "CBuffer.hpp"

namespace DEV {

void InitCBuffer(DeviceState& device, CBuffer& cb, size_t size)
{
	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	desc.ByteWidth = size;
	HOK( device.device->CreateBuffer( &desc, NULL, ~cb ));
}

} // namespace DEV