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
	state.context->ClearState();

	state.context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
	state.context->RSSetViewports( 1, &backbuffer.viewport );
	state.context->OMSetRenderTargets(1, &backbuffer.rtv, NULL);
	state.context->RSSetState(info.rs_default);
	state.context->VSSetShader(info.vs_noop, NULL, 0);
	state.context->GSSetShader(info.gs_fullscreen, NULL, 0);
	state.context->PSSetShader(info.ps_final, NULL, 0);
	state.context->PSSetConstantBuffers(0, 1, &cb_frame);
	ID3D11ShaderResourceView* resources[4] = { gbuffer0.srv, gbuffer1.srv, lbuffer.srv, zbuffer.srv };
	state.context->PSSetShaderResources(0, 4, resources);
	state.context->PSSetSamplers(0, 1, &info.sm_point);

	state.context->Draw( 1, 0 );
}

} // namespace Devora