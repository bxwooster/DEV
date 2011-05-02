#include "PlayerState.hpp"
#include "Transforms.hpp"
#include "Camera.hpp"

#include <d3dx10math.h>

namespace Devora {

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

	camera.view = view_axis * rotate * transforms[0].inverse();
}

} // namespace Devora