#include "PhysicsState.hpp"
#include "PlayerState.hpp"
#include "TimingData.hpp"
#include "Transforms.hpp"
#include "Camera.hpp"

#include "btBulletDynamicsCommon.h"
#include <d3dx10math.h>

namespace Devora {

void CrunchPhysics(PhysicsState& state, Transforms& transforms,
	PlayerState& player, TimingData& timing, Camera& camera)
{
	float yaw = float(player.camera.yaw / 180 * M_PI);
	Matrix2f rot;
	rot << cos(yaw), sin(yaw), -sin(yaw), cos(yaw);
	Vector2f dir = -rot * player.mov;

	btVector3 vel = state.playerBody->getLinearVelocity();
	if (player.jump) vel.setZ( 5 );
	
	state.playerBody->activate(true);
	state.playerBody->setLinearVelocity( btVector3(5 * dir.x(), 5 * dir.y(), vel.z()));

	state.dynamicsWorld->stepSimulation(timing.delta, 6);

	Matrix4f rotate;
	D3DXMatrixRotationYawPitchRoll
		( (D3DXMATRIX*) rotate.data(), 
		player.camera.pitch * float(M_PI / 180), 0.0, player.camera.yaw * float(M_PI / 180));
	rotate.transposeInPlace();

	Matrix4f view_axis;
	view_axis << 0, 1, 0, 0,
				 0, 0, 1, 0,
				 1, 0, 0, 0,
				 0, 0, 0, 1;

	state.eye.topLeftCorner<3,3>() = Matrix3f::Identity();
	camera.view = view_axis * rotate * state.eye.inverse();
}

} // namespace Devora