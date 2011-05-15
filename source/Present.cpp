#include "DeviceState.hpp"

#include "OK.h"

namespace DEV {

void Present(DeviceState& device)
{
	HOK( device.swap_chain->Present(0, 0) );
}

} // namespace DEV