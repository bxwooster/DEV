#include "OK.h"

#include "GraphicsState.hpp"

namespace Devora {

void Prepare(GraphicsState& state)
{
	HOK( state.var.aperture->SetFloat( state.aperture ) );
	HOK( state.var.z_near->SetFloat( state.z_near ) );
	HOK( state.var.field_of_view->SetFloat( state.field_of_view ) );
	HOK( state.var.aspect_ratio->SetFloat( state.aspect_ratio ) );
}

} // namespace Devora