#include "GraphicsState.hpp"
#include "VisualRenderInfo.hpp"
#include "Transforms.hpp"
#include "Visuals.hpp"
#include "Geometries.hpp"
#include "Camera.hpp"
#include "ZBuffer.hpp"
#include "Buffer.hpp"
#include "CBuffer.hpp"
#include "CBufferLayouts.hpp"

namespace Devora {

typedef unsigned int uint;

void RenderVisuals(GraphicsState& state, VisualRenderInfo& info, 
	Transforms& transforms, Visuals& visuals, Geometries& geometries, Camera& camera,
	Buffer& gbuffer0, Buffer& gbuffer1, ZBuffer& zbuffer, CBuffer& cb_object)
{
	OK( gbuffer0.viewport == gbuffer1.viewport);
	OK( gbuffer0.viewport == zbuffer.viewport);

	state->ClearState();

	ID3D11RenderTargetView* targets[] = { gbuffer0.rtv, gbuffer1.rtv };
	state->OMSetRenderTargets(2, targets, zbuffer.dsv);
	state->RSSetViewports( 1, &gbuffer0.viewport );
	state->IASetInputLayout( info.layout );
	state->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	state->OMSetBlendState( NULL, 0, 0xffffffff);
	state->OMSetDepthStencilState( NULL, 0 );
	state->RSSetState( info.rs_default );
	state->VSSetConstantBuffers(0, 1, &cb_object);
	state->VSSetShader(info.vs_render, NULL, 0);
	state->PSSetShader(info.ps_render, NULL, 0);

	for (uint i = 0; i < visuals.size(); i++)
	{
		Geometry& geom = geometries[visuals[i].geometry];

		CBufferLayouts::object data;
		data.world_view = camera.view * transforms[visuals[i].transform];
		data.world_view_proj = camera.proj * data.world_view;

		state->UpdateSubresource(cb_object, 0, NULL, (void*)&data, sizeof(data), 0);
		state->IASetVertexBuffers(0, 1, &geom.buffer, &geom.stride, &geom.offset);
		state->Draw( geom.count, 0 );
	}
}

} // namespace Devora9