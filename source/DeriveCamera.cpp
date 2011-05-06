#include "Transforms.hpp"
#include "PlayerState.hpp"
#include "DeviceState.hpp"
#include "Camera.hpp"

#include <d3dx10math.h>

namespace Devora {
namespace Tools {

void SetProjectionMatrix(Matrix4f& proj, float y_fov, float aspect_ratio, float z_near);

}; using namespace Tools;

void DeriveCamera(Transforms& transforms, PlayerState& player, DeviceState& device, Camera& camera)
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
	camera.xy_to_ray = Vector2f(-aspect_ratio, 1.0) * tan(alpha);

	camera.view = view_axis * rotate * transforms[0].inverse();
	SetProjectionMatrix(camera.proj, camera.vertical_fov, aspect_ratio, device.z_near);
}

} // namespace Devora