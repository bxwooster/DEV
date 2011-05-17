#include "Transforms.hpp"
#include "Visuals.hpp"
#include "Lights.hpp"
#include "Geometries.hpp"
#include "PhysicsState.hpp"
#include "DeviceState.hpp"
#include "Tools.hpp"

#include <iostream>
#include <btBulletDynamicsCommon.h>

namespace DEV {

class MotionState : public btMotionState
{
	Matrix4f& transform;

public:
	MotionState(Matrix4f& transform_) :
		transform(transform_)
	{}

	virtual void getWorldTransform(btTransform &t) const
	{
		*(Matrix4f*)&t = transform.transpose();
	}

	virtual void setWorldTransform(const btTransform& t)
	{
		transform = (*(Matrix4f*)&t).transpose();
		transform.col(3).w() = 1.0;
	}
};

void InitScene(Transforms& transforms, Visuals& visuals, Lights& lights,
	Geometries& geometries, PhysicsState& state, DeviceState& device)
{ 
	// Make A Scene oooo-oh-oo-oh-oo-oh-oo-oh-oo
	geometries.push_back( Tools::ReadGeometry(device.device, "geometry//Sphere") );

	Matrix4f view_axis;
	view_axis << 0, 1, 0, 0,
				 0, 0, 1, 0,
				 1, 0, 0, 0,
				 0, 0, 0, 1; //!
	view_axis = view_axis.inverse().eval();

	Matrix4f t = Matrix4f::Identity();
	t.col(3) = Vector4f(10, 0, 5, 1);

	Visual v;
	Light light;

	v.transform = transforms.size();
	v.geometry = 0;
	visuals.push_back( v ); //!

	transforms.push_back( t ); // player
	transforms.push_back( Matrix4f::Identity() ); // plane

	int N = 1;
	for (int i = -N; i <= N; i++)
	{
		for (int j = -N; j <= N; j++)
		{
			if (i == 0 && j == 0) continue;
			t.col(3) = Vector4f(i * 3.0f, j * 3.0f, 1.0f, 1.0f);
			v.transform = transforms.size();
			transforms.push_back( t );
			visuals.push_back( v );
		}
	}

	t.col(3) = Vector4f(0, 0, 1, 1);
	light.colour = Vector3f(0.5, 0.5, 0);
	v.transform = light.transform = transforms.size();
	transforms.push_back( t * view_axis );
	lights.point.push_back(light);
	visuals.push_back( v );

	//t.col(3) = Vector4f(6, 3, 1, 1);
	//transforms.push_back( t );
	//light.colour = Vector3f(1, 0, 0);
	//v.transform = light.transform = transforms.size();
	//transforms.push_back( t * view_axis );
	//lights.dir.push_back(light);
	//visuals.push_back( v );

	//t.col(3) = Vector4f(6, -3, 1, 1);
	//light.colour = Vector3f(0, 1, 0);
	//v.transform = light.transform = transforms.size();
	//transforms.push_back( t * view_axis );
	//lights.dir.push_back(light);
	//visuals.push_back( v );

	int n = transforms.size();
	state.bodies = (btRigidBody*)btRigidBody::operator new[](n * sizeof(btRigidBody));
	//! delete

	btScalar mass;
	btVector3 localInertia;
	btMotionState* motionState;

	mass = 3;
	localInertia = btVector3(0, 0, 0); // can't rotate
	motionState = new MotionState(transforms[0]);
	state.bodies[0] = btRigidBody(mass, motionState, state.sphere.get(), localInertia);
	state.dynamicsWorld->addRigidBody(&state.bodies[0]);
	
	mass = 0;
	motionState = new MotionState(transforms[1]);
	state.bodies[1] = btRigidBody(mass, motionState, state.plane.get(), localInertia);
	state.dynamicsWorld->addRigidBody(&state.bodies[1]);

	mass = 1;
	state.sphere->calculateLocalInertia(mass, localInertia);

	for (int i = 2; i < transforms.size(); i++)
	{	
		motionState = new MotionState(transforms[i]);
		btRigidBody body(mass, motionState, state.sphere.get(), localInertia);
		state.bodies[i] = body;
		state.dynamicsWorld->addRigidBody(&state.bodies[i]);
	}
}

} // namespace DEV