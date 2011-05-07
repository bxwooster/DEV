#include "DeviceState.hpp"

#include "OK.h"

namespace Devora {

void Present(DeviceState& device)
{
	HOK( device.swap_chain->Present(0, 0) );
}

} // namespace Devora