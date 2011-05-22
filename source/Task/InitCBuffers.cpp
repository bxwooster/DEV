#include "InitCBuffer.hpp"
#include "OK.hpp"

namespace DEV {

void InitCBuffer::run()
{
	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = size;
	HOK( device.device->CreateBuffer( &desc, NULL, ~cb ));
}

} // namespace DEV