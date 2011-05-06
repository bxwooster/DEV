#include "Transforms.hpp"
#include "GraphicsState.hpp"
#include "ZBuffer.hpp"
#include "Buffer.hpp"
#include "Camera.hpp"

#include "OK.h"

#include <d3dx11.h>

namespace Devora {

void PostProcess(GraphicsState& state, ZBuffer& zbuffer, 
	Buffer& gbuffer0, Buffer& gbuffer1,	Buffer& lbuffer, Buffer& backbuffer)
{
	state.context->ClearState();

	state.context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
	state.context->RSSetViewports( 1, &backbuffer.viewport );

	HOK( state.var.lbuffer->SetResource( lbuffer.srv ) );
	HOK( state.var.gbuffer0->SetResource( gbuffer0.srv ) );
	HOK( state.var.gbuffer1->SetResource( gbuffer1.srv ) );
	state.context->OMSetRenderTargets(1, &backbuffer.rtv, NULL);
	HOK( state.pass_final->Apply( 0, state.context ) );
	state.context->Draw( 1, 0 );
}

} // namespace Devora