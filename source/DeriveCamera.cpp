#include "Transforms.hpp"
#include "PlayerState.hpp"
#include "DeviceState.hpp"
#include "GraphicsState.hpp"
#include "Camera.hpp"
#include "CBuffer.hpp"
#include "CBufferLayouts.hpp"

#include <d3dx10math.h>

namespace Devora {
namespace Tools {

void SetProjectionMatrix(Matrix4f& proj, float y_fov, float aspect_ratio, float z_near);

}; using namespace Tools;

void DeriveCamera(Transforms& transforms, PlayerState& player, 
	DeviceState& device, Camera& camera, GraphicsState& state, CBuffer& cb_frame)
{
	Matrix4f rotate;
	D3DXMatrixRotationYawPitchRoll
		( (D3DXMATRIX*) rotate.data(), 
		player.camera.pitch * float(M_PI / 180), 0.0, player.camera.yaw * float(M_PI / 180));
	rotate.transposeInPlace();

	Matrix4f view_axis;
	view_axis << 0, 1, 0, 0,
				 0, 0, 1, 0,
				 1, 0, 0, 0,
				 0, 0, 0, 1; //!

	camera.aperture = 1.0f;
	camera.vertical_fov = 60;
	float aspect_ratio = float(device.width) / device.height;

	float alpha = float(camera.vertical_fov * M_PI / 180 * 0.5);
	Vector2f xy_to_ray = Vector2f(-aspect_ratio, 1.0) * tan(alpha);

	camera.view = view_axis * rotate * transforms[0].inverse();
	SetProjectionMatrix(camera.proj, camera.vertical_fov, aspect_ratio, device.z_near);

	HOK( state.var.aperture->SetFloat( camera.aperture ) );
	HOK( state.var.xy_to_ray->SetRawValue
		( (void*)xy_to_ray.data(), 0, sizeof(Vector2f) ) );
	Matrix4f view_i( camera.view.inverse() );
	HOK( state.var.view_i->SetMatrix( view_i.data() ));

	CBufferLayouts::frame data = 
		{ camera.view.inverse(), camera.aperture, xy_to_ray, device.z_near };

	state.context->UpdateSubresource(cb_frame, 0, NULL, (void*)&data, sizeof(data), 0);
}

} // namespace Devora