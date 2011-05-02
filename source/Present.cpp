#include "GraphicsState.hpp"

namespace Devora {

void Present(GraphicsState& state)
{
	HOK( state.swap_chain->Present(0, 0) );
}

} // namespace Devora