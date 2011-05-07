#include "Transforms.hpp"
#include "PlayerState.hpp"
#include "Camera.hpp"

#include <d3dx10math.h>

namespace Devora {
namespace Tools {

void SetProjectionMatrix(Matrix4f& proj, float y_fov, float aspect_ratio, float z_near);

}; using namespace Tools;

void DeriveCamera(Transforms& transforms, PlayerState& player, Camera& camera)
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
	camera.view = view_axis * rotate * transforms[0].inverse();
	SetProjectionMatrix(camera.proj, camera.vertical_fov, camera.aspect_ratio, camera.z_near);
}

} // namespace Devora