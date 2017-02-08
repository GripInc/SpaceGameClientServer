#include "manager/StateManager.h"

#include "manager/LoggerManager.h"

#include "model/Ship.h"

namespace
{
	const std::string LOG_CLASS_TAG = "StateManager";
}

void StateManager::setMaxStateAmount(unsigned int _maxStateAmount)
{
	mMaxStateAmount = _maxStateAmount;
}

void StateManager::saveState(const SectorState& _state)
{
	long long index = _state.mTick - mTickOffset;
	if (index < mStates.size())
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "saveState", "Replacing state " + StringUtils::toStr(_state.mTick) + " at index " + StringUtils::toStr(index), false);
		mStates[(size_t)index] = _state;
	}
	else if (index == mStates.size())
	{
		mStates.push_back(_state);

		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "saveState", "Adding state " + StringUtils::toStr(_state.mTick) + " at index " + StringUtils::toStr(index), false);

		while (mStates.size() > mMaxStateAmount)
		{
			mStates.pop_front();
			mTickOffset++;
		}
	}
	else
		LoggerManager::getInstance().logE(LOG_CLASS_TAG, "saveState", "index:" + StringUtils::toStr(index) + " mTickOffset:" + StringUtils::toStr(mTickOffset) + "; _state.mTick:" + StringUtils::toStr(_state.mTick) + "; mStates.back().mTick:" + StringUtils::toStr(mStates.back().mTick) + "; mStates.size():" + StringUtils::toStr(mStates.size()));
}

bool StateManager::getState(SectorTick _tick, SectorState& _outState) const
{
	size_t index = (size_t)(_tick - mTickOffset);
	if (index < mStates.size())
	{
		_outState = mStates[index];
		return true;
	}
	else
	{
		LoggerManager::getInstance().logE(LOG_CLASS_TAG, "getState", "No state found for index:" + StringUtils::toStr(index) + ", mTickOffset:" + StringUtils::toStr(mTickOffset) + "; _tick:" + StringUtils::toStr(_tick) + "; mStates.size():" + StringUtils::toStr(mStates.size()));
		return false;
	}
}

bool StateManager::getShipState(SectorTick _tick, UniqueId _uniqueId, ShipState& _outState) const
{
	SectorState sectorState;
	
	if (getState(_tick, sectorState))
	{
		for (std::list<ShipState>::const_reference shipState : sectorState.mShipsState)
		{
			if (shipState.mUniqueId == _uniqueId)
			{
				_outState = shipState;
				return true;
			}
		}

		LoggerManager::getInstance().logE(LOG_CLASS_TAG, "getShipState", "Found no state for _uniqueId " + StringUtils::toStr(_uniqueId) + "; _tick:" + StringUtils::toStr(_tick));
		return false;
	}
	else
		return false;
}

void DynamicObjectState::serialize(RakNet::BitStream& _bitStream) const
{
	_bitStream.Write(mWorldTransform);
	_bitStream.Write(mLinearVelocity);
	_bitStream.Write(mAngularVelocity);
	_bitStream.Write(mTotalForce);
	_bitStream.Write(mTotalTorque);
}

void DynamicObjectState::deserialize(RakNet::BitStream& _bitStream)
{
	_bitStream.Read(mWorldTransform);
	_bitStream.Read(mLinearVelocity);
	_bitStream.Read(mAngularVelocity);
	_bitStream.Read(mTotalForce);
	_bitStream.Read(mTotalTorque);
}

void ShipState::serialize(RakNet::BitStream& _bitStream) const
{
	DynamicObjectState::serialize(_bitStream);

	_bitStream.Write(mUniqueId);

	_bitStream.Write(mCurrentRollForce);
	_bitStream.Write(mCurrentYawForce);
	_bitStream.Write(mCurrentPitchForce);
	_bitStream.Write(mEngineWantedThrust);
	_bitStream.Write(mEngineRealThrust);

	size_t hardpointsAmount = mHarpointsState.size();
	_bitStream.Write(hardpointsAmount);
	for (size_t i = 0; i < hardpointsAmount; ++i)
	{
		_bitStream.Write(mHarpointsState[i].first);
		_bitStream.Write(mHarpointsState[i].second);
	}
}

void ShipState::deserialize(RakNet::BitStream& _bitStream)
{
	DynamicObjectState::deserialize(_bitStream);

	_bitStream.Read(mUniqueId);

	_bitStream.Read(mCurrentRollForce);
	_bitStream.Read(mCurrentYawForce);
	_bitStream.Read(mCurrentPitchForce);
	_bitStream.Read(mEngineWantedThrust);
	_bitStream.Read(mEngineRealThrust);

	size_t hardpointsAmount;
	_bitStream.Read(hardpointsAmount);
	mHarpointsState.clear();
	mHarpointsState.resize(hardpointsAmount, std::make_pair(0, 0.f));
	for (size_t i = 0; i < hardpointsAmount; ++i)
	{
		_bitStream.Read(mHarpointsState[i].first);
		_bitStream.Read(mHarpointsState[i].second);
	}
}

void ShotState::serialize(RakNet::BitStream& _bitStream) const
{
	_bitStream.Write(mWorldTransform);
	_bitStream.Write(mLinearVelocity);
}

void ShotState::deserialize(RakNet::BitStream& _bitStream)
{
	_bitStream.Read(mWorldTransform);
	_bitStream.Read(mLinearVelocity);
}

void SectorState::serialize(RakNet::BitStream& _bitStream) const
{
	_bitStream.Write(mTick);

	size_t shipsStateAmount = mShipsState.size();
	_bitStream.Write(shipsStateAmount);
	for (std::list<ShipState>::const_reference shipState : mShipsState)
		shipState.serialize(_bitStream);

	size_t shotsStateAmount = mShotsState.size();
	_bitStream.Write(shotsStateAmount);
	for (std::list<ShotState>::const_reference shotState : mShotsState)
		shotState.serialize(_bitStream);
}

void SectorState::deserialize(RakNet::BitStream& _bitStream)
{
	mShipsState.clear();
	mShotsState.clear();

	_bitStream.Read(mTick);

	size_t shipsStateAmount;
	_bitStream.Read(shipsStateAmount);
	for (size_t i = 0; i < shipsStateAmount; ++i)
	{
		mShipsState.push_back(ShipState());
		mShipsState.back().deserialize(_bitStream);
	}

	size_t shotsStateAmount;
	_bitStream.Read(shotsStateAmount);
	for (size_t i = 0; i < shotsStateAmount; ++i)
	{
		mShotsState.push_back(ShotState());
		mShotsState.back().deserialize(_bitStream);
	}
}
