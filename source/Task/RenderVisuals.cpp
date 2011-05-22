#include "RenderVisuals.hpp"
#include "CBufferLayouts.hpp"

namespace DEV {

typedef unsigned int uint;

void RenderVisuals::run()
{
	OK( gbuffer.viewport == zbuffer.viewport);

	state->ClearState();

	ID3D11Buffer* buffers[] = {cb_frame, cb_object};
	ID3D11RenderTargetView* targets[] = { gbuffer.rtv };
	ID3D11UnorderedAccessView* uavs[] = { oit_start.uav, oit_scattered.uav, oit_consolidated.uav };
	unsigned int counts[] = { -1, 1, -1 };

	state->OMSetRenderTargetsAndUnorderedAccessViews
		(1, targets, zbuffer.dsv, 1, 2, uavs, counts);
	state->RSSetViewports( 1, &gbuffer.viewport );
	state->IASetInputLayout( info.layout );
	state->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	state->OMSetBlendState( NULL, 0, 0xffffffff);
	state->OMSetDepthStencilState( NULL, 0 );
	state->RSSetState( info.rs_default );

	state->VSSetShader(info.vs_render, NULL, 0);
	state->PSSetShader(info.ps_render, NULL, 0);

	state->VSSetConstantBuffers(0, 1, &cb_object);
	state->PSSetConstantBuffers(0, 2, buffers);

	CBufferLayouts::object data;
	for (uint i = 0; i < visuals.size(); i++)
	{
		Geometry& geom = geometries[visuals[i].geometry];

		data.world_view = camera.view * transforms[visuals[i].transform];
		data.world_view_proj = camera.proj * data.world_view;
		data.__colour = Vector4f(1, 1, 1, 0.8f);

		state->UpdateSubresource(cb_object, 0, NULL, (void*)&data, sizeof(data), 0);
		state->IASetVertexBuffers(0, 2, &*geom.buffers, geom.strides, geom.offsets);
		state->IASetIndexBuffer( geom.buffers[2], DXGI_FORMAT_R16_UINT, 0 );
		state->DrawIndexed( geom.count, 0, 0 );
	}

	data.world_view = camera.view * transforms[1]; //! plane
	data.world_view_proj = camera.proj * data.world_view;
	data.__colour = Vector4f(1, 1, 1, 1);

	state->UpdateSubresource(cb_object, 0, NULL, (void*)&data, sizeof(data), 0);

	state->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
	state->VSSetShader(info.vs_noop, NULL, 0);
	state->GSSetShader(info.gs_infinite_plane, NULL, 0);
	state->GSSetConstantBuffers(0, 1, &cb_object);
	state->Draw( 1, 0 );
}

} // namespace DEV