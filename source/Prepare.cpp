#include "Camera.hpp"
#include "GraphicsState.hpp"
#include "ZBuffer.hpp"
#include "CBuffer.hpp"
#include "CBufferLayouts.hpp"

#include "OK.h"

namespace Devora {

void Prepare(GraphicsState& state, CBuffer& cb_frame, ZBuffer& zbuffer, Camera& camera)
{
	float alpha = float(camera.vertical_fov * M_PI / 180 * 0.5);

	CBufferLayouts::frame data;
	data.aperture = camera.aperture;
	data.view_i = camera.view.inverse();
	data.xy_to_ray = Vector2f(-camera.aspect_ratio, 1.0) * tan(alpha);
	data.z_near = camera.z_near;
	data.rcpres = Vector2f( 1 / float(camera.screen.w), 1 / float(camera.screen.h) );
	state->UpdateSubresource(cb_frame, 0, NULL, (void*)&data, sizeof(data), 0);

	state->ClearDepthStencilView(zbuffer.dsv, D3D11_CLEAR_DEPTH, 1.0, 0);
}

} // namespace Devora