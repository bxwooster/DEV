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
	Matrix4f t = Matrix4f::Identity();
	t.col(3) = Vector4f(5, 5, 5, 1);

	Visual v = { transforms.size(), 1 };
	transforms.push_back( t ); // player
	visuals.push_back( v ); // 

	v.index = transforms.size();
	v.type = 0;
	transforms.push_back( Matrix4f::Identity() );
	visuals.push_back( v ); // plane

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

	t.col(3) = Vector4f(0, 0, 5, 1);
	Light light = { Vector3f(1, 1, 0), transforms.size() };
	Visual visual = { transforms.size(), 1 };
	transforms.push_back( t );
	lights.point.push_back(light);
	visuals.push_back( visual );

	t.col(3) = Vector4f(0, 7, 1, 1);
	transforms.push_back( t );
	visual.index = light.index = transforms.size();
	light.colour = Vector3f(1, 0, 0);
	transforms.push_back( t );
	lights.dir.push_back(light);
	visuals.push_back( visual );

	t.col(3) = Vector4f(0, -7, 1, 1);
	transforms.push_back( t );
	visual.index = light.index = transforms.size();
	light.colour = Vector3f(0, 1, 0);
	transforms.push_back( t );
	lights.dir.push_back(light);
	visuals.push_back( visual );

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

	mass = 5;
	localInertia = btVector3(0, 0, 0); // can't rotate
	btMotionState* motionState = new MotionState(transforms[0]);
	state.playerBody = std::unique_ptr<btRigidBody>
		(new btRigidBody(mass, motionState, state.sphere.get(), localInertia) );
	state.dynamicsWorld->addRigidBody(state.playerBody.get());
}

} // namespace Devora