#include "Transforms.hpp"
#include "Visuals.hpp"
#include "Lights.hpp"
#include "PhysicsState.hpp"

#include <btBulletDynamicsCommon.h>

namespace Devora {

namespace {
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
}

void InitScene(Transforms& transforms, Visuals& visuals, Lights& lights, PhysicsState& state)
{
	transforms.push_back( Matrix4f::Identity() );
	Visual v = { 0, 0 };
	visuals.push_back( v );

	Matrix4f t = Matrix4f::Identity();
	for (int i = -2; i <= 2; i++)
	{
		for (int j = -2; j <= 2; j++)
		{
			t.col(3) = Vector4f(i * 3.0f + j % 2, j * 3.0f + i % 2, 1.0f, 1.0f);
			transforms.push_back( t );
			Visual v = { transforms.size() - 1, 1 };
			visuals.push_back( v );
		}
	}

	t = Matrix4f::Identity();

	t.col(3) = Vector4f(0, 0, 5, 1);
	transforms.push_back( t );
	Light light = { Vector3f(1, 0, 0), transforms.size() - 1, LightType::point };
	lights.push_back(light);

	t.col(3) = Vector4f(0, 7, 1, 1);
	transforms.push_back( t );
	Light light2 = { Vector3f(0, 0, 1), transforms.size() - 1, LightType::directional };
	lights.push_back(light2);

	t.col(3) = Vector4f(0, -7, 1, 1);
	transforms.push_back( t );
	Light light3 = { Vector3f(0, 1, 0), transforms.size() - 1, LightType::directional };
	lights.push_back(light);

	btScalar mass(1);
	btVector3 localInertia;
	state.sphere->calculateLocalInertia(mass, localInertia);

	for (int i = 1; i < transforms.size(); i++)
	{	
		btMotionState* motionState = new MotionState(transforms[i]);
		btRigidBody* body = new btRigidBody(mass, motionState, state.sphere, localInertia);
		state.dynamicsWorld->addRigidBody(body);
		state.bodies.push_back(body);
	}

	mass = 5;
	state.sphere->calculateLocalInertia(mass, localInertia);
	btMotionState* motionState = new MotionState(state.eye);
	state.playerBody = new btRigidBody(mass, motionState, state.sphere, localInertia);
	state.dynamicsWorld->addRigidBody(state.playerBody);
}

} // namespace Devora