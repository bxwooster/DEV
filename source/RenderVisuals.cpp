#include "GraphicsState.hpp"
#include "VisualRenderInfo.hpp"
#include "Visuals.hpp"
#include "Camera.hpp"
#include "Transforms.hpp"
#include "ZBuffer.hpp"
#include "Buffer.hpp"

#include <d3dx11.h>

namespace Devora {

typedef unsigned int uint;

void RenderVisuals(GraphicsState& state, VisualRenderInfo& info, 
	Transforms& transforms, Visuals& visuals, Camera& camera,
	Buffer& gbuffer0, Buffer& gbuffer1, ZBuffer& zbuffer)
{
	ID3D11RenderTargetView* targets[] = { gbuffer0.rtv, gbuffer1.rtv };
	state.context->OMSetRenderTargets(2, targets, zbuffer.dsv);
	state.context->RSSetViewports( 1, &gbuffer0.viewport );
	state.context->IASetInputLayout( info.layout );
	state.context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	for (uint i = 0; i < visuals.size(); i++)
	{
		Visual& visual = visuals[i];

		Matrix4f world_view = camera.view * transforms[visual.index];
		Matrix4f world_view_proj = info.proj * world_view;
	
		HOK( state.var.world_view->SetMatrix( world_view.data() ));
		HOK( state.var.world_view_proj->SetMatrix( world_view_proj.data() ));
		HOK( state.pass_render->Apply( 0, state.context ) );
		
		Geometry geom = info.geoms[visual.type];
		state.context->IASetVertexBuffers(0, 1, &geom.buffer, &geom.stride, &geom.offset);
		
		state.context->Draw( geom.count, 0 );
	}
}

} // namespace Devora