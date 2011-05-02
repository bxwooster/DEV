#include "OK.h"

#include "GraphicsState.hpp"
#include "Buffer.hpp"
#include "ZBuffer.hpp"
#include "VisualRenderInfo.hpp"
#include "Camera.hpp"

namespace Devora {

void Prepare(GraphicsState& state, Buffer& gbuffer0, Buffer& gbuffer1,
	Buffer& lbuffer, ZBuffer& zbuffer, Camera& camera)
{
	state.context->ClearState();

	const float black[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	state.context->ClearRenderTargetView(gbuffer0.rtv, black);
	state.context->ClearRenderTargetView(gbuffer1.rtv, black);
	state.context->ClearRenderTargetView(lbuffer.rtv, black);
	state.context->ClearDepthStencilView(zbuffer.dsv, D3D11_CLEAR_DEPTH, 1.0, 0);
	
	Matrix4f view_i( camera.view.inverse() );
	HOK( state.var.view_i->SetMatrix( view_i.data() ));
	HOK( state.var.view->SetMatrix( camera.view.data() ));

	Vector3f colour(0.1f, 0.1f, 0.1f);
	HOK( state.var.light_colour->SetRawValue
		( (void*)colour.data(), 0, sizeof(Vector3f) ) );

	HOK( state.var.aperture->SetFloat( state.aperture ) );
	HOK( state.var.z_near->SetFloat( state.z_near ) );
	HOK( state.var.field_of_view->SetFloat( state.field_of_view ) );
	HOK( state.var.aspect_ratio->SetFloat( state.aspect_ratio ) );
}

} // namespace Devora