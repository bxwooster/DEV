#include "Present.hpp"
#include "OK.hpp"

namespace DEV {

void Present::run()
{
	HOK( device.swap_chain->Present(0, 0) );
}

} // namespace DEV