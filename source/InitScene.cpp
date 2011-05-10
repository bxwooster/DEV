#include "Transforms.hpp"
#include "Visuals.hpp"
#include "Lights.hpp"
#include "Geometries.hpp"
#include "PhysicsState.hpp"
#include "DeviceState.hpp"
#include "Tools.hpp"

#include <btBulletDynamicsCommon.h>

namespace Devora {

class MotionState : public btMotionState
{
	Matrix4f& transform;

public:
	MotionState(Matrix4f& transform_) :
		transform(transform_)
	{}

	virtual ~MotionState()
	{}

	virtual void getWorldTransform(btTransform &t) const
	{
		Matrix4f transposed = transform.transpose();
		t.setFromOpenGLMatrix( (btScalar*)transposed.data() );
	}

	virtual void setWorldTransform(const btTransform& t)
	{
		t.getOpenGLMatrix( (btScalar*)transform.data() );
		transform.transposeInPlace();
	}
};

void InitScene(Transforms& transforms, Visuals& visuals, Lights& lights,
	Geometries& geometries, PhysicsState& state, DeviceState& device)
{
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

	//for (int i = -2; i <= 2; i++)
	//{
	//	for (int j = -2; j <= 2; j++)
	//	{
	//		t.col(3) = Vector4f(i * 3.0f + j % 2, j * 3.0f + i % 2, 1.0f, 1.0f);
	//		Visual v = { transforms.size(), 1 };
	//		transforms.push_back( t );
	//		visuals.push_back( v );
	//	}
	//}

	//t.col(3) = Vector4f(0, 0, 5, 1);
	//light.colour = Vector3f(1, 1, 0);
	//v.transform = light.transform = transforms.size();
	//transforms.push_back( t * view_axis );
	//lights.point.push_back(light);
	//visuals.push_back( v );

	//t.col(3) = Vector4f(5, 3, 1, 1);
	//transforms.push_back( t );
	//light.colour = Vector3f(1, 0, 0);
	//v.transform = light.transform = transforms.size();
	//transforms.push_back( t * view_axis );
	//lights.dir.push_back(light);
	//visuals.push_back( v );

	t.col(3) = Vector4f(5, -3, 1, 1);
	light.colour = Vector3f(0, 1, 0);
	v.transform = light.transform = transforms.size();
	transforms.push_back( t * view_axis );
	lights.dir.push_back(light);
	visuals.push_back( v );


	btScalar mass(1);
	btVector3 localInertia;
	state.sphere->calculateLocalInertia(mass, localInertia);

	for (int i = 2; i < transforms.size(); i++)
	{	
		btMotionState* motionState = new MotionState(transforms[i]);
		btRigidBody* body = new btRigidBody(mass, motionState, state.sphere.get(), localInertia);
		state.dynamicsWorld->addRigidBody(body);
		state.bodies.push_back(body);
	}

	mass = 3;
	localInertia = btVector3(0, 0, 0); // can't rotate
	btMotionState* motionState = new MotionState(transforms[0]);
	state.playerBody = std::unique_ptr<btRigidBody>
		(new btRigidBody(mass, motionState, state.sphere.get(), localInertia) );
	state.dynamicsWorld->addRigidBody(state.playerBody.get());
}

} // namespace Devora