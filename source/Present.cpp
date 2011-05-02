#include "DeviceState.hpp"

namespace Devora {

void Present(DeviceState& state)
{
	HOK( state.swap_chain->Present(0, 0) );
}

} // namespace Devora