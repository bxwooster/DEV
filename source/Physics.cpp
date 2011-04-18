#include "Physics.h"

Physics::Physics()
{
	auto collisionConfiguration = new btDefaultCollisionConfiguration();
	auto dispatcher = new btCollisionDispatcher(collisionConfiguration);
	dispatcher->registerCollisionCreateFunc(BOX_SHAPE_PROXYTYPE,BOX_SHAPE_PROXYTYPE, collisionConfiguration->getCollisionAlgorithmCreateFunc(CONVEX_SHAPE_PROXYTYPE,CONVEX_SHAPE_PROXYTYPE));
	auto broadphase = new btDbvtBroadphase();
	auto solver = new btSequentialImpulseConstraintSolver();

	auto dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->getSolverInfo().m_splitImpulse=true;
	dynamicsWorld->getSolverInfo().m_numIterations = 20;
	dynamicsWorld->getDispatchInfo().m_useContinuous=true;
	dynamicsWorld->setGravity(btVector3(0, 0, -10));

	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 0, 1), 0);

	btTransform groundTransform;
	groundTransform.setIdentity();

	btScalar mass(0.);
	btVector3 localInertia(0,0,0);

	auto myMotionState = new btDefaultMotionState(groundTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
	auto body = new btRigidBody(rbInfo);

	dynamicsWorld->addRigidBody(body);
}