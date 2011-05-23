#include "DeriveCamera.hpp"
#include "Tools.hpp"
#include <d3dx10math.h>

namespace DEV {

void DeriveCamera::run()
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
	camera.view = view_axis * rotate * transforms[0].inverse(); //! player
	Tools::SetProjectionMatrix(camera.proj, camera.vertical_fov, camera.aspect_ratio, camera.z_near);
}

} // namespace DEV