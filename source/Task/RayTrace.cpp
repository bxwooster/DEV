#include "RayTrace.hpp"
#include "CBufferLayouts.hpp"

namespace DEV {

typedef unsigned int uint;

void RayTrace::run()
{
	OK( gbuffer.viewport == zbuffer.viewport);

	state->ClearState();

	CBufferLayouts::tracy data;
	data.steps = 64;
	data.interp = 0.95f;
	data.threshold = 0.0025f;
	data.eps = 1e-3f;
	data.view = camera.view;
	data.viewI = camera.view.inverse();
	data.eye = -data.viewI.topLeftCorner<3, 3>() * camera.view.col(3).head<3>();

	ID3D11RenderTargetView* targets[] = { gbuffer.rtv };
	ID3D11Buffer* buffers[] = { cb_frame, cb_tracy };

	state->UpdateSubresource(cb_tracy, 0, NULL, (void*)&data, sizeof(data), 0);

	state->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
	state->OMSetRenderTargets(1, targets, zbuffer.dsv);
	state->RSSetViewports( 1, &gbuffer.viewport );

	state->RSSetState(info.rs_default);
	
	state->VSSetShader(info.vs_noop, NULL, 0);
	state->GSSetShader(info.gs_fullscreen, NULL, 0);
	state->PSSetShader(info.ps_tracy, NULL, 0);
	
	state->PSSetConstantBuffers(0, 2, buffers);

	state->Draw( 1, 0 );
}

} // namespace DEV