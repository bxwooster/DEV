#include "Transforms.hpp"
#include "GraphicsState.hpp"
#include "VisualRenderInfo.hpp"
#include "Visuals.hpp"
#include "Camera.hpp"
#include "ZBuffer.hpp"
#include "Buffer.hpp"
#include "CBuffer.hpp"
#include "CBufferLayouts.hpp"

#include <d3dx11.h>

namespace Devora {

typedef unsigned int uint;

void RenderVisuals(GraphicsState& state, VisualRenderInfo& info, 
	Transforms& transforms, Visuals& visuals, Camera& camera,
	Buffer& gbuffer0, Buffer& gbuffer1, ZBuffer& zbuffer, CBuffer& cb_object)
{
	OK( gbuffer0.viewport == gbuffer1.viewport);
	OK( gbuffer0.viewport == zbuffer.viewport);

	state.context->ClearState();
	state.context->ClearDepthStencilView(zbuffer.dsv, D3D11_CLEAR_DEPTH, 1.0, 0);

	ID3D11RenderTargetView* targets[] = { gbuffer0.rtv, gbuffer1.rtv };
	state.context->OMSetRenderTargets(2, targets, zbuffer.dsv);
	state.context->RSSetViewports( 1, &gbuffer0.viewport );
	state.context->IASetInputLayout( info.layout );
	state.context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	state.context->OMSetBlendState( NULL, 0, 0xffffffff);
	state.context->OMSetDepthStencilState( NULL, 0 );
	state.context->RSSetState( info.rs_default );
	state.context->VSSetConstantBuffers(1, 1, &cb_object);
	state.context->VSSetShader(info.vs_render, NULL, 0);
	state.context->PSSetShader(info.ps_render, NULL, 0);

	for (uint i = 0; i < visuals.size(); i++)
	{
		Geometry& geom = info.geoms[visuals[i].type];

		CBufferLayouts::object data;
		data.world_view = camera.view * transforms[visuals[i].index];
		data.world_view_proj = camera.proj * data.world_view;

		state.context->UpdateSubresource(cb_object, 0, NULL, (void*)&data, sizeof(data), 0);
		state.context->IASetVertexBuffers(0, 1, &geom.buffer, &geom.stride, &geom.offset);
		state.context->Draw( geom.count, 0 );
	}
}

} // namespace Devora9