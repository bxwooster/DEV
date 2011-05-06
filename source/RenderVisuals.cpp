#include "Transforms.hpp"
#include "GraphicsState.hpp"
#include "VisualRenderInfo.hpp"
#include "Visuals.hpp"
#include "Camera.hpp"
#include "ZBuffer.hpp"
#include "Buffer.hpp"

#include <d3dx11.h>

namespace Devora {

typedef unsigned int uint;

void RenderVisuals(GraphicsState& state, VisualRenderInfo& info, 
	Transforms& transforms, Visuals& visuals, Camera& camera,
	Buffer& gbuffer0, Buffer& gbuffer1, ZBuffer& zbuffer)
{
	state.context->ClearState();

	const float black[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	state.context->ClearRenderTargetView(gbuffer0.rtv, black);
	state.context->ClearRenderTargetView(gbuffer1.rtv, black);
	state.context->ClearDepthStencilView(zbuffer.dsv, D3D11_CLEAR_DEPTH, 1.0, 0);

	HOK( state.var.aperture->SetFloat( camera.aperture ) );
	HOK( state.var.xy_to_ray->SetRawValue
		( (void*)camera.xy_to_ray.data(), 0, sizeof(Vector2f) ) );

	ID3D11RenderTargetView* targets[] = { gbuffer0.rtv, gbuffer1.rtv };
	state.context->OMSetRenderTargets(2, targets, zbuffer.dsv);
	OK( gbuffer0.viewport == gbuffer1.viewport );
	state.context->RSSetViewports( 1, &gbuffer0.viewport );
	state.context->IASetInputLayout( info.layout );
	state.context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	for (uint i = 0; i < visuals.size(); i++)
	{
		Matrix4f world_view = camera.view * transforms[visuals[i].index];
		Matrix4f world_view_proj = camera.proj * world_view;
	
		HOK( state.var.world_view->SetMatrix( world_view.data() ));
		HOK( state.var.world_view_proj->SetMatrix( world_view_proj.data() ));
		HOK( state.pass_render->Apply( 0, state.context ) );
		
		Geometry& geom = info.geoms[visuals[i].type];
		state.context->IASetVertexBuffers(0, 1, &geom.buffer, &geom.stride, &geom.offset);
		
		state.context->Draw( geom.count, 0 );
	}
}

} // namespace Devora