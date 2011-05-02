#include "GraphicsState.hpp"
#include "PostProcessInfo.hpp"
#include "Transforms.hpp"
#include "ZBuffer.hpp"
#include "Buffer.hpp"

#include "OK.h"

#include <d3dx11.h>

namespace Devora {

void PostProcess(GraphicsState& state, PostProcessInfo& info, ZBuffer& zbuffer, 
	Buffer& gbuffer0, Buffer& gbuffer1,	Buffer& lbuffer, Buffer& backbuffer)
{
	HOK( state.var.light_colour->SetRawValue
		( (void*)info.ambient.data(), 0, sizeof(Vector3f) ) );
	HOK( state.var.lbuffer->SetResource( lbuffer.srv ) );
	HOK( state.var.gbuffer0->SetResource( gbuffer0.srv ) );
	HOK( state.var.gbuffer1->SetResource( gbuffer1.srv ) );
	state.context->OMSetRenderTargets(1, &backbuffer.rtv, NULL);
	HOK( state.pass_final->Apply( 0, state.context ) );
	state.context->Draw( 1, 0 );
}

} // namespace Devora