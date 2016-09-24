#ifndef _STATE_MANAGER_H_
#define _STATE_MANAGER_H_

#include <vector>

#include "SpaceGameTypes.h"
#include "model/InputState.h"
#include "BitStream.h"
#include "btBulletDynamicsCommon.h"

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

	virtual void serialize(RakNet::BitStream& _bitStream) const
	{
		_bitStream.Write(mWorldTransform);
		_bitStream.Write(mLinearVelocity);
		_bitStream.Write(mAngularVelocity);
		_bitStream.Write(mTotalForce);
		_bitStream.Write(mTotalTorque);
	}

	virtual void deserialize(RakNet::BitStream& _bitStream)
	{
		_bitStream.Read(mWorldTransform);
		_bitStream.Read(mLinearVelocity);
		_bitStream.Read(mAngularVelocity);
		_bitStream.Read(mTotalForce);
		_bitStream.Read(mTotalTorque);
	}

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

	ShipState(const btRigidBody* _rigidBody, float _currentRollForce, float _currentYawForce, float _currentPitchForce, float _enginePotentialForce, const std::vector<std::pair<int, float> >& _harpointsState)
		: DynamicObjectState(_rigidBody),
		mCurrentRollForce(_currentRollForce),
		mCurrentYawForce(_currentYawForce),
		mCurrentPitchForce(_currentPitchForce),
		mEnginePotentialForce(_enginePotentialForce),
		mHarpointsState(_harpointsState)
	{}

	virtual void serialize(RakNet::BitStream& _bitStream) const
	{
		DynamicObjectState::serialize(_bitStream);

		_bitStream.Write(mCurrentRollForce);
		_bitStream.Write(mCurrentYawForce);
		_bitStream.Write(mCurrentPitchForce);
		_bitStream.Write(mEnginePotentialForce);

		size_t hardpointsAmount = mHarpointsState.size();
		_bitStream.Write(hardpointsAmount);
		for(size_t i = 0; i < hardpointsAmount; ++i)
		{
			_bitStream.Write(mHarpointsState[i].first);
			_bitStream.Write(mHarpointsState[i].second);
		}
	}

	virtual void deserialize(RakNet::BitStream& _bitStream)
	{
		DynamicObjectState::deserialize(_bitStream);

		_bitStream.Read(mCurrentRollForce);
		_bitStream.Read(mCurrentYawForce);
		_bitStream.Read(mCurrentPitchForce);
		_bitStream.Read(mEnginePotentialForce);

		size_t hardpointsAmount;
		_bitStream.Read(hardpointsAmount);
		mHarpointsState.clear();
		mHarpointsState.resize(hardpointsAmount, std::make_pair(0, 0.f));
		for(size_t i = 0; i < hardpointsAmount; ++i)
		{
			_bitStream.Read(mHarpointsState[i].first);
			_bitStream.Read(mHarpointsState[i].second);
		}
	}

	//Systems relative
	float mCurrentRollForce = 0.f;
	float mCurrentYawForce = 0.f;
	float mCurrentPitchForce = 0.f;

	float mEnginePotentialForce = 0.f;

	//Hardpoints (index, elapsedTime)
	std::vector<std::pair<int, float> > mHarpointsState;
};

template<typename T>
class StateManager
{
public:
	void saveState(SectorTick _tick, const T& _state)
	{
		mStates[_tick] = _state;

		/*if((*mStates.begin()).first < _tick - MAX_SECTOR_TICK_REWIND_AMOUNT)
		{
			SectorTick minimumTick = _tick - MAX_SECTOR_TICK_REWIND_AMOUNT;
			std::map<SectorTick, T>::const_iterator it = mStates.begin();
			std::map<SectorTick, T>::const_iterator itEnd = mStates.end();
			while((*it).first < minimumTick)
			{
				++it;
			}
			mStates.erase(mStates.begin(), it);
		}*/
	}

	bool getState(SectorTick _tick, T& _outState)
	{
		std::map<SectorTick, T>::const_iterator foundIt = mStates.find(_tick);

		if(foundIt == mStates.end())
		{
			//TODO log anomaly
			return false;
		}
		else
		{
			_outState = (*foundIt).second;
			return true;
		}
	}

protected:
	std::map<SectorTick, T> mStates;
};

#endif //_STATE_MANAGER_H_