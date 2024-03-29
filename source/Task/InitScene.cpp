#include "InitScene.hpp"
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

void InitScene::run()
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
	t.col(3) = Vector4f(10, 0, 1, 1);

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
			t.col(3) = Vector4f(i * 3, j * 3, 1, 1);
			v.transform = transforms.size();
			transforms.push_back( t );
			visuals.push_back( v );
		}
	}

	t.col(3) = Vector4f(0, 0, 1, 1);
	light.colour = Vector3f(0.5f, 0.5f, 0);
	v.transform = light.transform = transforms.size();
	transforms.push_back( t * view_axis );
	lights.point.push_back(light);
	visuals.push_back( v );

	t.col(3) = Vector4f(8, 4, 1, 1);
	transforms.push_back( t );
	light.colour = Vector3f(1, 0, 0);
	v.transform = light.transform = transforms.size();
	transforms.push_back( t * view_axis );
	lights.dir.push_back(light);
	visuals.push_back( v );

	t.col(3) = Vector4f(8, -4, 1, 1);
	light.colour = Vector3f(0, 1, 0);
	v.transform = light.transform = transforms.size();
	transforms.push_back( t * view_axis );
	lights.dir.push_back(light);
	visuals.push_back( v );

	size_t size = transforms.size() * sizeof(btRigidBody);
	state.bodies = btRigidBody::operator new[](size);

	btScalar mass;
	btVector3 localInertia;
	btMotionState* motionState;

	mass = 3;
	localInertia = btVector3(0, 0, 0); // can't rotate
	motionState = new MotionState(transforms[0]);
	state.bodies[0] = btRigidBody(mass, motionState, state.sphere, localInertia);
	state.dynamicsWorld->addRigidBody(&state.bodies[0]);
	
	mass = 0;
	motionState = new MotionState(transforms[1]);
	state.bodies[1] = btRigidBody(mass, motionState, state.plane, localInertia);
	state.dynamicsWorld->addRigidBody(&state.bodies[1]);

	mass = 1;
	state.sphere->calculateLocalInertia(mass, localInertia);

	for (int i = 2; i < transforms.size(); i++)
	{	
		motionState = new MotionState(transforms[i]);

		state.bodies[i] = btRigidBody(mass, motionState, state.sphere, localInertia);

		state.bodies[i].setCcdMotionThreshold(1e-7);
		state.bodies[i].setCcdSweptSphereRadius(0.45); //!?

		state.dynamicsWorld->addRigidBody(&state.bodies[i]);
	}
}

} // namespace DEV