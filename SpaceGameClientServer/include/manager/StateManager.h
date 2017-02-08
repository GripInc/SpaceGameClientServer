#ifndef _STATE_MANAGER_H_
#define _STATE_MANAGER_H_

#include <vector>
#include <list>
#include <deque>

#include "SpaceGameTypes.h"

#include "model/InputState.h"

#include "BitStream.h"
#include "btBulletDynamicsCommon.h"

class Ship;

//Class used to record dynamic object state by ticks
class DynamicObjectState
{
public:
	DynamicObjectState() {}

	DynamicObjectState(const btRigidBody* _rigidBody)
		: mWorldTransform(_rigidBody->getWorldTransform()),
		mLinearVelocity(_rigidBody->getLinearVelocity()),
		mAngularVelocity(_rigidBody->getAngularVelocity()),
		mTotalForce(_rigidBody->getTotalForce()),
		mTotalTorque(_rigidBody->getTotalTorque())
	{}

	virtual void serialize(RakNet::BitStream& _bitStream) const;
	virtual void deserialize(RakNet::BitStream& _bitStream);

	btTransform mWorldTransform;
	btVector3 mLinearVelocity;
	btVector3 mAngularVelocity;
	btVector3 mTotalForce;
	btVector3 mTotalTorque;
};

class ShipState : public DynamicObjectState
{
public:
	ShipState() {}

	ShipState(UniqueId _uniqueId, float _currentRollForce, float _currentYawForce, float _currentPitchForce, float _engineWantedThrust, float _engineRealThrust, const std::vector<std::pair<int, float> >& _harpointsState)
		: mUniqueId(_uniqueId),
		mCurrentRollForce(_currentRollForce),
		mCurrentYawForce(_currentYawForce),
		mCurrentPitchForce(_currentPitchForce),
		mEngineWantedThrust(_engineWantedThrust),
		mEngineRealThrust(_engineRealThrust),
		mHarpointsState(_harpointsState)
	{}

	virtual void serialize(RakNet::BitStream& _bitStream) const;
	virtual void deserialize(RakNet::BitStream& _bitStream);

	UniqueId mUniqueId;

	//Systems relative
	float mCurrentRollForce = 0.f;
	float mCurrentYawForce = 0.f;
	float mCurrentPitchForce = 0.f;

	float mEngineWantedThrust = 0.f; 
	float mEngineRealThrust = 0.f;

	//Hardpoints (index, elapsedTime)
	std::vector<std::pair<int, float> > mHarpointsState;
};

class ShotState
{
public:
	ShotState() {}

	ShotState(const btTransform& _worldTransform, const btVector3& _linearVelocity)
		: mWorldTransform(_worldTransform),
		mLinearVelocity(_linearVelocity)
	{}

	virtual void serialize(RakNet::BitStream& _bitStream) const;
	virtual void deserialize(RakNet::BitStream& _bitStream);

	btTransform mWorldTransform;
	btVector3 mLinearVelocity;
};

class SectorState
{
public:
	SectorState() {}

	SectorState(SectorTick _tick, const std::list<ShipState>& _shipsState, const std::list<ShotState>& _shotsState)
		: mTick(_tick),
		mShipsState(_shipsState),
		mShotsState(_shotsState)
	{}

	virtual void serialize(RakNet::BitStream& _bitStream) const;
	virtual void deserialize(RakNet::BitStream& _bitStream);

	SectorTick mTick = 0U;
	std::list<ShipState> mShipsState;
	std::list<ShotState> mShotsState;
};

class StateManager
{
public:
	void setMaxStateAmount(unsigned int _maxStateAmount);

	void saveState(const SectorState& _state);
	bool getState(SectorTick _tick, SectorState& _outState) const;

	bool getShipState(SectorTick _tick, UniqueId _uniqueId, ShipState& _outState) const;

protected:
	std::deque<SectorState> mStates;
	long long int mTickOffset = 0;
	unsigned int mMaxStateAmount = 0U;
};

#endif //_STATE_MANAGER_H_
