#include "model/DynamicObject.h"

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

#include "utils/OgreBulletConvert.h"

//const float DynamicObject::MAX_ACCEPTABLE_VECTOR_3D_DELTA = 0.1f;
//const float DynamicObject::MAX_ACCEPTABLE_QUATERNION_DELTA = 0.1f;

void DynamicObject::instantiateCollisionObject()
{
	if(mCompoundShape != NULL)
	{
		const DynamicObjectSettings* dynamicObjectSettings = static_cast<const DynamicObjectSettings*>(mObjectSettings);

		btTransform startTransform = btTransform(convert(dynamicObjectSettings->mInitialOrientation), convert(dynamicObjectSettings->mInitialPosition));
		mMyMotionState = new MyMotionState(mSceneNode, startTransform);
		mRigidBody = createRigidBody(startTransform, mCompoundShape, mMyMotionState, dynamicObjectSettings->mMass, getInertia());
		mRigidBody->setRestitution(DEFAULT_RESTITUTION_VALUE);
		mRigidBody->setActivationState(DISABLE_DEACTIVATION);
		mRigidBody->setDamping(dynamicObjectSettings->mLinearDamping, dynamicObjectSettings->mAngularDamping);
		mRigidBody->setCollisionFlags(mRigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK); //CF_CUSTOM_MATERIAL_CALLBACK allows the trigger of MyContactCallback on contact added
		mDynamicWorld->addRigidBody(mRigidBody);
	}
}

void DynamicObject::destroy()
{
	//StaticObject::destroy();
}

void DynamicObject::saveState(SectorTick _tick)
{
	mStateManager.saveState(_tick, DynamicObjectState(mRigidBody));
}

void DynamicObject::overrideState(SectorTick _tick, const DynamicObjectState& _dynamicObjectState)
{
	mStateManager.saveState(_tick, _dynamicObjectState);
}

void DynamicObject::setState(SectorTick _tick)
{
	DynamicObjectState dynamicObjectState;
	mStateManager.getState(_tick, dynamicObjectState);

	mRigidBody->setWorldTransform(dynamicObjectState.mWorldTransform);
	mRigidBody->setLinearVelocity(dynamicObjectState.mLinearVelocity);
	mRigidBody->setAngularVelocity(dynamicObjectState.mAngularVelocity);
	mRigidBody->applyCentralForce(dynamicObjectState.mTotalForce);
	mRigidBody->applyTorque(dynamicObjectState.mTotalTorque);
}

//bool DynamicObject::checkAndFixState(const State& _state)
//{
	/*const State& currentState = getState(_state.mTick);

	bool result = compareStates(currentState, _state);

	//If too much difference we fix it
	if(!result)
	{
		mRigidBody->clearForces();
		mRigidBody->setLinearVelocity(_state.mLinearVelocity);
		mRigidBody->setAngularVelocity(_state.mAngularVelocity);
		mRigidBody->applyCentralForce(_state.mTotalForce);
		mRigidBody->applyTorque(_state.mTotalTorque);
		this->forceWorldTransform(_state.mWorldTransform);
	}
	
	return result;*/
	//return true;
//}

/*bool DynamicObject::compareStates(const State& _state1, const State& _state2) const
{
	return	(fabs(_state1.mWorldTransform.getOrigin().length2()	- _state2.mWorldTransform.getOrigin().length2())	< MAX_ACCEPTABLE_VECTOR_3D_DELTA
		&&	fabs(_state1.mWorldTransform.getRotation().length2()- _state2.mWorldTransform.getRotation().length2())	< MAX_ACCEPTABLE_QUATERNION_DELTA
		&&	fabs(_state1.mLinearVelocity.length2()				- _state2.mLinearVelocity.length2())				< MAX_ACCEPTABLE_VECTOR_3D_DELTA
		&&	fabs(_state1.mAngularVelocity.length2()				- _state2.mAngularVelocity.length2())				< MAX_ACCEPTABLE_VECTOR_3D_DELTA
		&&	fabs(_state1.mTotalForce.length2()					- _state2.mTotalForce.length2())					< MAX_ACCEPTABLE_VECTOR_3D_DELTA
		&&	fabs(_state1.mTotalTorque.length2()					- _state2.mTotalTorque.length2())					< MAX_ACCEPTABLE_VECTOR_3D_DELTA);
}*/

/*const DynamicObject::State& DynamicObject::getState(SectorTick _tick)
{
	if(_tick < mStates.front().mTick)
	{
		//TODO WTF
		//Or not? Maybe a delayed message? Or we should test tick before?
		std::vector<int> crash;
		crash[1];

		return mStates.front();
	}

	while(_tick > mStates.front().mTick)
	{
		mStates.pop();
	}

	return mStates.front();
}*/