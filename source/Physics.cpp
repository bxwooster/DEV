#include "Physics.h"

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

Physics::Physics()
{
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	dispatcher->registerCollisionCreateFunc(BOX_SHAPE_PROXYTYPE,BOX_SHAPE_PROXYTYPE, collisionConfiguration->getCollisionAlgorithmCreateFunc(CONVEX_SHAPE_PROXYTYPE,CONVEX_SHAPE_PROXYTYPE));
	broadphase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->getSolverInfo().m_splitImpulse = true;
	dynamicsWorld->getSolverInfo().m_numIterations = 20;
	dynamicsWorld->getDispatchInfo().m_useContinuous = true;
	dynamicsWorld->setGravity(btVector3(0, 0, -10));

	groundShape = new btStaticPlaneShape(btVector3(0, 0, 1), 0);

	btTransform groundTransform;
	groundTransform.setIdentity();

	btScalar mass(0);
	btVector3 localInertia(0, 0, 0);
		
	auto motionState = new btDefaultMotionState(groundTransform); //!
	auto body = new btRigidBody(mass, motionState, groundShape, localInertia);
	dynamicsWorld->addRigidBody(body);
}

void Physics::capture(btAlignedObjectArray<Matrix4f>& transforms)
{
	btSphereShape* sphere = new btSphereShape(1); //!

	for (int i = 1; i < transforms.size(); i++)
	{
		btScalar mass(1);
		btVector3 localInertia;
		sphere->calculateLocalInertia(mass, localInertia);
		
		auto motionState = new MotionState(transforms[i]);
		auto body = new btRigidBody(mass, motionState, sphere, localInertia);
		dynamicsWorld->addRigidBody(body);

		body->setCcdMotionThreshold(1e-7);
		body->setCcdSweptSphereRadius(0.9);
	}
};

void Physics::step(double dt)
{
	dynamicsWorld->stepSimulation(btScalar(dt));
}