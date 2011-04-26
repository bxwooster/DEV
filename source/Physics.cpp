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
		
	btMotionState* motionState = new btDefaultMotionState(groundTransform);
	btRigidBody* planeBody = new btRigidBody(mass, motionState, groundShape, localInertia);
	dynamicsWorld->addRigidBody(planeBody);
}

void Physics::capture(btAlignedObjectArray<Matrix4f>& transforms, Matrix4f& eye)
{
	btSphereShape* sphere = new btSphereShape(1);
	btScalar mass(1);
	btVector3 localInertia;
	sphere->calculateLocalInertia(mass, localInertia);

	for (int i = 1; i < transforms.size(); i++)
	{	
		btMotionState* motionState = new MotionState(transforms[i]);
		btRigidBody* body = new btRigidBody(mass, motionState, sphere, localInertia);
		dynamicsWorld->addRigidBody(body);
	}

	mass = 5;
	sphere->calculateLocalInertia(mass, localInertia);
	btMotionState* motionState = new MotionState(eye);
	playerBody = new btRigidBody(mass, motionState, sphere, localInertia);
	dynamicsWorld->addRigidBody(playerBody);
};

void Physics::control_player()
{
	//playerBody->applyCentralForce( btVector3(0.0, 5.0, 0.0) ); ?
}

void Physics::step(double dt)
{
	dynamicsWorld->stepSimulation(btScalar(dt), 6);
}