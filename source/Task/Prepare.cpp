#include "Prepare.hpp"
#include "CBufferLayouts.hpp"
#include "OK.hpp"

namespace DEV {

void Prepare::run()
{
	float alpha = float(camera.vertical_fov * M_PI / 180 * 0.5);

	CBufferLayouts::frame data;
	data.aperture = camera.aperture;
	data.view_i = camera.view.inverse();
	data.xy_to_ray = Vector2f(-camera.aspect_ratio, 1.0) * tan(alpha);
	data.z_near = camera.z_near;
	data.rcpres = Vector2f( 1 / float(camera.screen.w), 1 / float(camera.screen.h) );
	data.res = Vector2f( float(camera.screen.w), float(camera.screen.h) );
	data.light_scale = 8.0f;
	state->UpdateSubresource(cb_frame, 0, NULL, (void*)&data, sizeof(data), 0);

	state->ClearDepthStencilView(zbuffer.dsv, D3D11_CLEAR_DEPTH, 1.0, 0);

	unsigned int null[4] = { 0 };
	state->ClearUnorderedAccessViewUint(oit_start.uav, null);
}

} // namespace DEV