#include "CrunchPhysics.hpp"
#include <algorithm>
#include "BulletCollision/CollisionDispatch/btSimulationIslandManager.h"

namespace DEV {

static float const fixedTimeStep = 1.0f / 60.0f;

void ClearForces(btDiscreteDynamicsWorld* world)
{
	// for each dynamic body: force = 0
	for (int i = 0; i < world->m_nonStaticRigidBodies.size(); ++i)
	{
		btRigidBody* body = world->m_nonStaticRigidBodies[i];
		body->clearForces();
	}
}

void ApplyGravity(btDiscreteDynamicsWorld* world)
{
	// for each active dynamic body: force += mass * gravity
	for (int i = 0; i < world->m_nonStaticRigidBodies.size(); ++i)
	{
		btRigidBody* body = world->m_nonStaticRigidBodies[i];
		if (body->isActive())
			body->applyGravity();
	}
}

void PredictMotion(btDiscreteDynamicsWorld* world)
{
	// for each dynamic body: calculate velocities, calculate predicted transform
	for (int i = 0; i < world->m_nonStaticRigidBodies.size(); ++i)
	{
		btRigidBody* body = world->m_nonStaticRigidBodies[i];
		if (!body->isStaticOrKinematicObject())
		{
			body->integrateVelocities(fixedTimeStep);
			body->applyDamping(fixedTimeStep);
			body->predictIntegratedTransform(fixedTimeStep, body->getInterpolationWorldTransform());
		}
	}
}

void UpdateAABBs(btDiscreteDynamicsWorld* world, btDispatcher* dispatcher, btBroadphaseInterface* broadphase)
{
	// for all collision objects: calculate AABBs
	btTransform predictedTrans;
	for (int i = 0; i < world->m_collisionObjects.size(); i++)
	{
		btCollisionObject* colObj = world->m_collisionObjects[i];

		//! need to init separately
		// if (m_forceUpdateAllAabbs || colObj->isActive())
		{
			btVector3 minAabb, maxAabb;
			colObj->getCollisionShape()->getAabb(colObj->getWorldTransform(), minAabb, maxAabb);

			// continuous:
			btVector3 minAabb2, maxAabb2;
			colObj->getCollisionShape()->getAabb(colObj->getInterpolationWorldTransform(), minAabb2, maxAabb2);
			minAabb.setMin(minAabb2);
			maxAabb.setMax(maxAabb2);

			btVector3 contactThreshold(
				gContactBreakingThreshold,
				gContactBreakingThreshold,
				gContactBreakingThreshold);

			minAabb -= contactThreshold;
			maxAabb += contactThreshold;

			broadphase->setAabb(colObj->getBroadphaseHandle(), minAabb, maxAabb, dispatcher); 
		}
	}
}

void CalculateSimulationIslands(btDiscreteDynamicsWorld* world,
	btDispatcher* dispatcher, btSimulationIslandManager* island_manager)
{
	island_manager->updateActivationState(world, dispatcher);

	for (int i = 0; i < world->m_constraints.size() ; i++ )
	{
		btTypedConstraint* constraint = world->m_constraints[i];

		const btRigidBody* colObj0 = &constraint->getRigidBodyA();
		const btRigidBody* colObj1 = &constraint->getRigidBodyB();

		if (((colObj0) && (!(colObj0)->isStaticOrKinematicObject())) &&
			((colObj1) && (!(colObj1)->isStaticOrKinematicObject())))
		{
			if (colObj0->isActive() || colObj1->isActive())
			{
				island_manager->getUnionFind().unite((colObj0)->getIslandTag(),
					(colObj1)->getIslandTag());
			}
		}
	}

	//Store the island id in each body
	island_manager->storeIslandActivationState(world);
}

class btClosestNotMeConvexResultCallback : public btCollisionWorld::ClosestConvexResultCallback
{
public:

	btCollisionObject* m_me;
	btScalar m_allowedPenetration;
	btOverlappingPairCache* m_pairCache;
	btDispatcher* m_dispatcher;

public:
	btClosestNotMeConvexResultCallback (btCollisionObject* me,const btVector3& fromA,const btVector3& toA,btOverlappingPairCache* pairCache,btDispatcher* dispatcher) : 
	  btCollisionWorld::ClosestConvexResultCallback(fromA,toA),
		m_me(me),
		m_allowedPenetration(0.0f),
		m_pairCache(pairCache),
		m_dispatcher(dispatcher)
	{ }

	virtual btScalar addSingleResult(btCollisionWorld::LocalConvexResult& convexResult,bool normalInWorldSpace)
	{
		if (convexResult.m_hitCollisionObject == m_me)
			return 1.0f;

		//ignore result if there is no contact response
		if(!convexResult.m_hitCollisionObject->hasContactResponse())
			return 1.0f;

		btVector3 linVelA,linVelB;
		linVelA = m_convexToWorld-m_convexFromWorld;
		linVelB = btVector3(0,0,0);//toB.getOrigin()-fromB.getOrigin();

		btVector3 relativeVelocity = (linVelA-linVelB);
		//don't report time of impact for motion away from the contact normal (or causes minor penetration)
		if (convexResult.m_hitNormalLocal.dot(relativeVelocity)>=-m_allowedPenetration)
			return 1.f;

		return ClosestConvexResultCallback::addSingleResult (convexResult, normalInWorldSpace);
	}

	virtual bool needsCollision(btBroadphaseProxy* proxy0) const
	{
		//don't collide with itself
		if (proxy0->m_clientObject == m_me)
			return false;

		///don't do CCD when the collision filters are not matching
		if (!ClosestConvexResultCallback::needsCollision(proxy0))
			return false;

		btCollisionObject* otherObj = (btCollisionObject*) proxy0->m_clientObject;

		//call needsResponse, see http://code.google.com/p/bullet/issues/detail?id=179
		if (m_dispatcher->needsResponse(m_me,otherObj))
		{
			return true;
		}
		return false;
	}
};

void IntegrateTransforms(btDiscreteDynamicsWorld* world, btBroadphaseInterface* broadphase, btDispatcher* dispatcher)
{
	btTransform predictedTrans;
	for (int i = 0; i < world->m_nonStaticRigidBodies.size(); ++i)
	{
		btRigidBody* body = world->m_nonStaticRigidBodies[i];
		body->setHitFraction(1.f);

		if (body->isActive() && (!body->isStaticOrKinematicObject()))
		{
			body->predictIntegratedTransform(fixedTimeStep, predictedTrans);
			
			btScalar squareMotion = (predictedTrans.getOrigin()-body->getWorldTransform().getOrigin()).length2();

			if (body->getCcdSquareMotionThreshold() && body->getCcdSquareMotionThreshold() < squareMotion)
			{
				if (body->getCollisionShape()->isConvex())
				{
					btClosestNotMeConvexResultCallback sweepResults(
						body,
						body->getWorldTransform().getOrigin(),
						predictedTrans.getOrigin(),
						broadphase->getOverlappingPairCache(),
						dispatcher);

					btSphereShape tmpSphere(body->getCcdSweptSphereRadius());
					sweepResults.m_allowedPenetration = world->getDispatchInfo().m_allowedCcdPenetration;

					sweepResults.m_collisionFilterGroup = body->getBroadphaseProxy()->m_collisionFilterGroup;
					sweepResults.m_collisionFilterMask  = body->getBroadphaseProxy()->m_collisionFilterMask;
					btTransform modifiedPredictedTrans = predictedTrans;
					modifiedPredictedTrans.setBasis(body->getWorldTransform().getBasis());

					world->convexSweepTest(&tmpSphere,body->getWorldTransform(),modifiedPredictedTrans,sweepResults);

					if (sweepResults.hasHit() && (sweepResults.m_closestHitFraction < 1.f))
					{
						// clamp
						body->predictIntegratedTransform(fixedTimeStep*sweepResults.m_closestHitFraction, predictedTrans);
						body->proceedToTransform( predictedTrans);

						//response between two dynamic objects without friction, assuming 0 penetration depth
						btScalar appliedImpulse = 0.f;
						btScalar depth = 0.f;
						appliedImpulse = resolveSingleCollision(
							body,
							sweepResults.m_hitCollisionObject,
							sweepResults.m_hitPointWorld,
							sweepResults.m_hitNormalWorld,
							world->getSolverInfo(),
							depth);

						continue;
					}
				}
			}
			
			body->proceedToTransform(predictedTrans);
		}
	}
}

void SynchronizeMotionStates(btDiscreteDynamicsWorld* world, float timeToSimulate)
{
	for (int i = 0; i < world->m_nonStaticRigidBodies.size(); ++i)
	{
		btRigidBody* body = world->m_nonStaticRigidBodies[i];
		if (body->isActive())
		{
			if (body->getMotionState() && !body->isStaticOrKinematicObject())
			{
				//we need to call the update at least once, even for sleeping objects
				//otherwise the 'graphics' transform never updates properly
				///@todo: add 'dirty' flag
				//if (body->getActivationState() != ISLAND_SLEEPING)
				{
					btTransform interpolatedTransform;
					btTransformUtil::integrateTransform(
						body->getInterpolationWorldTransform(),
						body->getInterpolationLinearVelocity(),
						body->getInterpolationAngularVelocity(),
						timeToSimulate, // * body->getHitFraction(), //!?
						interpolatedTransform);

					body->getMotionState()->setWorldTransform(interpolatedTransform); //!
				}
			}
		}
	}
}

void CalculateOverlappingPairs(btDbvtBroadphase* broadphase, btDispatcher* dispatcher)
{
	broadphase->performDeferredRemoval(dispatcher);
}

class btCollisionPairCallback : public btOverlapCallback
{
	const btDispatcherInfo& m_dispatchInfo;
	btCollisionDispatcher*	m_dispatcher;

public:

	btCollisionPairCallback(const btDispatcherInfo& dispatchInfo, btCollisionDispatcher* dispatcher) :
		m_dispatchInfo(dispatchInfo),
		m_dispatcher(dispatcher)
	{ }

	virtual ~btCollisionPairCallback() {}

	virtual bool processOverlap(btBroadphasePair& pair)
	{
		(*m_dispatcher->getNearCallback())(pair,*m_dispatcher,m_dispatchInfo);

		return false;
	}
};

void DispatchAllCollisionPairs(btDiscreteDynamicsWorld* world, btDbvtBroadphase* broadphase, btCollisionDispatcher* dispatcher)
{
	btCollisionPairCallback	collisionCallback(world->getDispatchInfo(), dispatcher);
	world->m_broadphasePairCache->getOverlappingPairCache()->processAllOverlappingPairs(&collisionCallback, dispatcher);
}

inline int btGetConstraintIslandId(const btTypedConstraint* lhs)
{	
	const btCollisionObject& rcolObj0 = lhs->getRigidBodyA();
	const btCollisionObject& rcolObj1 = lhs->getRigidBodyB();
	return rcolObj0.getIslandTag() >= 0 ? rcolObj0.getIslandTag() : rcolObj1.getIslandTag();
}

struct InplaceSolverIslandCallback : public btSimulationIslandManager::IslandCallback
{

	btContactSolverInfo&	m_solverInfo;
	btConstraintSolver*		m_solver;
	btTypedConstraint**		m_sortedConstraints;
	int						m_numConstraints;
	btIDebugDraw*			m_debugDrawer;
	btStackAlloc*			m_stackAlloc;
	btDispatcher*			m_dispatcher;
		
	btAlignedObjectArray<btCollisionObject*> m_bodies;
	btAlignedObjectArray<btPersistentManifold*> m_manifolds;
	btAlignedObjectArray<btTypedConstraint*> m_constraints;


	InplaceSolverIslandCallback(
		btContactSolverInfo& solverInfo,
		btConstraintSolver*	solver,
		btTypedConstraint** sortedConstraints,
		int	numConstraints,
		btIDebugDraw*	debugDrawer,
		btStackAlloc*			stackAlloc,
		btDispatcher* dispatcher)
		:m_solverInfo(solverInfo),
		m_solver(solver),
		m_sortedConstraints(sortedConstraints),
		m_numConstraints(numConstraints),
		m_debugDrawer(debugDrawer),
		m_stackAlloc(stackAlloc),
		m_dispatcher(dispatcher)
	{

	}


	InplaceSolverIslandCallback& operator=(InplaceSolverIslandCallback& other)
	{
		btAssert(0);
		(void)other;
		return *this;
	}
	virtual	void	ProcessIsland(btCollisionObject** bodies,int numBodies,btPersistentManifold**	manifolds,int numManifolds, int islandId)
	{
		if (islandId<0)
		{
			if (numManifolds + m_numConstraints)
			{
				///we don't split islands, so all constraints/contact manifolds/bodies are passed into the solver regardless the island id
				m_solver->solveGroup( bodies,numBodies,manifolds, numManifolds,&m_sortedConstraints[0],m_numConstraints,m_solverInfo,m_debugDrawer,m_stackAlloc,m_dispatcher);
			}
		} else
		{
				//also add all non-contact constraints/joints for this island
			btTypedConstraint** startConstraint = 0;
			int numCurConstraints = 0;
			int i;
				
			//find the first constraint for this island
			for (i=0;i<m_numConstraints;i++)
			{
				if (btGetConstraintIslandId(m_sortedConstraints[i]) == islandId)
				{
					startConstraint = &m_sortedConstraints[i];
					break;
				}
			}
			//count the number of constraints in this island
			for (;i<m_numConstraints;i++)
			{
				if (btGetConstraintIslandId(m_sortedConstraints[i]) == islandId)
				{
					numCurConstraints++;
				}
			}

			if (m_solverInfo.m_minimumSolverBatchSize<=1)
			{
				///only call solveGroup if there is some work: avoid virtual function call, its overhead can be excessive
				if (numManifolds + numCurConstraints)
				{
					m_solver->solveGroup( bodies,numBodies,manifolds, numManifolds,startConstraint,numCurConstraints,m_solverInfo,m_debugDrawer,m_stackAlloc,m_dispatcher);
				}
			} else
			{
					
				for (i=0;i<numBodies;i++)
					m_bodies.push_back(bodies[i]);
				for (i=0;i<numManifolds;i++)
					m_manifolds.push_back(manifolds[i]);
				for (i=0;i<numCurConstraints;i++)
					m_constraints.push_back(startConstraint[i]);
				if ((m_constraints.size()+m_manifolds.size())>m_solverInfo.m_minimumSolverBatchSize)
				{
					processConstraints();
				} else
				{
					//printf("deferred\n");
				}
			}
		}
	}
	void	processConstraints()
	{
		if (m_manifolds.size() + m_constraints.size()>0)
		{
			m_solver->solveGroup( &m_bodies[0],m_bodies.size(), &m_manifolds[0], m_manifolds.size(), &m_constraints[0], m_constraints.size() ,m_solverInfo,m_debugDrawer,m_stackAlloc,m_dispatcher);
		}
		m_bodies.resize(0);
		m_manifolds.resize(0);
		m_constraints.resize(0);

	}

};

class btSortConstraintOnIslandPredicate
{
	public:

		bool operator() ( const btTypedConstraint* lhs, const btTypedConstraint* rhs )
		{
			int rIslandId0,lIslandId0;
			rIslandId0 = btGetConstraintIslandId(rhs);
			lIslandId0 = btGetConstraintIslandId(lhs);
			return lIslandId0 < rIslandId0;
		}
};

void SolveConstraints(btDiscreteDynamicsWorld* world, btSimulationIslandManager* island_manager)
{
	btContactSolverInfo& solverInfo = world->getSolverInfo();
	solverInfo.m_timeStep = fixedTimeStep; // actually used

	int n_constraints = world->m_constraints.size();

	//sorted version of all btTypedConstraint, based on islandId
	btAlignedObjectArray<btTypedConstraint*> sortedConstraints;
	sortedConstraints.resize(n_constraints);

	for (int i = 0; i < n_constraints; ++i)
	{
		sortedConstraints[i] = world->m_constraints[i];
	}

	sortedConstraints.quickSort(btSortConstraintOnIslandPredicate());
	
	btTypedConstraint** constraintsPtr = n_constraints ? &sortedConstraints[0] : NULL;
	
	InplaceSolverIslandCallback	solverCallback(	solverInfo,	world->m_constraintSolver, constraintsPtr, sortedConstraints.size(),
		world->m_debugDrawer, world->m_stackAlloc, world->m_dispatcher1);
	
	world->m_constraintSolver->prepareSolve(world->getNumCollisionObjects(), world->getDispatcher()->getNumManifolds());
	
	/// solve all the constraints for this island
	island_manager->buildAndProcessIslands(world->getDispatcher(), world,&solverCallback);

	solverCallback.processConstraints();

	world->m_constraintSolver->allSolved(solverInfo, world->m_debugDrawer, world->m_stackAlloc);
}

void CrunchPhysics::run()
{
	btVector3 vel = state.bodies[0].getLinearVelocity();
	if (player.jump) vel.setZ( 5 );

	float mult = player.sprint ? 15.0f : 5.0f;

	state.bodies[0].activate(true);
	state.bodies[0].setLinearVelocity( btVector3(mult * player.mov.x(), mult * player.mov.y(), vel.z()));
	
	//state.dynamicsWorld->stepSimulation(timing.delta, 6);
	//return;

	int const maxSubSteps = 6;

	//fixed timestep with interpolation
	state.timeToSimulate += timing.delta;
	int subSteps = std::min(int(state.timeToSimulate / fixedTimeStep), maxSubSteps);
	state.timeToSimulate -= subSteps * fixedTimeStep;

	btDiscreteDynamicsWorld* world = state.dynamicsWorld;

	ApplyGravity(world);

	for (int k = 0; k < subSteps; k++)
	{
		PredictMotion(world);
		UpdateAABBs(world, state.dispatcher, state.broadphase);
		CalculateOverlappingPairs(state.broadphase, state.dispatcher);
		DispatchAllCollisionPairs(world, state.broadphase, state.dispatcher);

		//! world->addSpeculativeContacts(fixedTimeStep);

		CalculateSimulationIslands(world, state.dispatcher, world->m_islandManager);
		
		SolveConstraints(world, world->m_islandManager);
		IntegrateTransforms(world, state.broadphase, state.dispatcher);

		//! world->updateActivationState(fixedTimeStep);
	}

	SynchronizeMotionStates(world, state.timeToSimulate);
	ClearForces(world);
}

} // namespace DEV
