#include "Transforms.hpp"
#include "GraphicsState.hpp"
#include "ZBuffer.hpp"
#include "Buffer.hpp"
#include "CBuffer.hpp"
#include "Camera.hpp"
#include "PostProcessInfo.hpp"

#include "OK.h"

#include <d3dx11.h>

namespace Devora {

void PostProcess(GraphicsState& state, ZBuffer& zbuffer, PostProcessInfo& info,
	Buffer& gbuffer0, Buffer& gbuffer1,	Buffer& lbuffer, Buffer& backbuffer, CBuffer& cb_frame)
{
	state->ClearState();

	ID3D11ShaderResourceView* resources[6] = { gbuffer0.srv, gbuffer1.srv, zbuffer.srv, NULL, NULL, lbuffer.srv };
	
	state->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
	state->RSSetViewports( 1, &backbuffer.viewport );
	state->OMSetRenderTargets(1, &backbuffer.rtv, NULL);
	state->RSSetState(info.rs_default);
	state->VSSetShader(info.vs_noop, NULL, 0);
	state->GSSetShader(info.gs_fullscreen, NULL, 0);
	state->PSSetShader(info.ps_final, NULL, 0);
	state->PSSetConstantBuffers(0, 1, &cb_frame);
	state->PSSetShaderResources(0, 6, resources);
	state->PSSetSamplers(0, 1, &info.sm_point);

	state->Draw( 1, 0 );
}

} // namespace Devora