#include "model/ClientSector.h"

#include "model/StaticObject.h"
#include "model/DynamicObject.h"
#include "model/GameSettings.h"
#include "model/ObjectPart.h"
#include "model/HardPoint.h"

#include "controller/SectorController.h"
#include "controller/ShipInputHandler.h"

#include "utils/StringUtils.h"
#include "utils/OgreUtils.h"
#include "utils/OgreBulletConvert.h"
#include "utils/BulletDebugDraw.h"

#include "manager/LoggerManager.h"

#include "OgreSceneNode.h"

#include "BitStream.h"

namespace
{
	const std::string LOG_CLASS_TAG = "ClientSector";
}

UniqueId ClientSector::sTemporaryUniqueId = -1;

//void ClientSector::instantiateShip(const std::string& _shipId, const Ogre::Quaternion& _orientation, const Ogre::Vector3& _position, RakNet::RakNetGUID _rakNetGUID)
//{
//	const ShipSettings* shipSettings = GameSettings::getInstance().getShip(_shipId);
//
//	if (shipSettings)
//	{
//		Ship* newShip = new Ship();
//		newShip->initShip(shipSettings);
//		newShip->instantiateObject(mSceneManager, mDynamicWorld, getTemporaryNextUniqueId());
//		newShip->forceWorldTransform(btTransform(convert(_orientation), convert(_position)));
//		mShips[_rakNetGUID] = newShip;
//
//		//Add neutral input
//		mLastClientsInput[_rakNetGUID] = InputState();
//	}
//	else
//	{
//		LoggerManager::getInstance().logE(LOG_CLASS_TAG, "instantiateShip", "No ship settings found for _shipId : " + _shipId);
//		assert(false);
//	}
//}

void ClientSector::instantiatePlayerShip(Ship& _playerShip, const Ogre::Quaternion& _orientation, const Ogre::Vector3& _position, UniqueId _uniqueId, RakNet::RakNetGUID _rakNetGUID, Ogre::SceneNode* _cameraSceneNode)
{
	mPlayerShip = &_playerShip;
	mPlayerShip->init(mSceneManager, mDynamicWorld, _uniqueId);
	mPlayerShip->instantiateObject();
	mPlayerShip->attachCamera(_cameraSceneNode);
	mPlayerShip->forceWorldTransform(btTransform(convert(_orientation), convert(_position)));
	mShips[_rakNetGUID] = mPlayerShip;
}

void ClientSector::updateSector(ShipInputHandler& _shipInputHandler)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "START at tick " + StringUtils::toStr(mSectorTick), false, true);

	//Always add last input so when simulating we can use it (we don't want to use default input)
	addPlayerInputInHistory(_shipInputHandler.mInputState);

	_shipInputHandler.sendInputToServer(mSectorTick);
	
	if (!mLastReceivedSectorState.mSimulated)
	{
		mLastReceivedSectorState.mSimulated = true;
		SectorTick resimulateFromTick = mLastReceivedSectorState.mLastAcknowledgedInput;

		//Set ship states as received from server
		for (std::map<RakNet::RakNetGUID, ShipState>::const_reference pair : mLastReceivedSectorState.mShips)
		{
			//Find the ship in sector
			std::map<RakNet::RakNetGUID, Ship*>::const_iterator foundShip = mShips.find(pair.first);
			if (foundShip != mShips.end())
			{
				LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "Setting ship state at tick : " + StringUtils::toStr(resimulateFromTick), false);

				Ship* ship = (*foundShip).second;
				ship->overrideSavedState(resimulateFromTick, pair.second);
				ship->setState(resimulateFromTick);
			}
			else
			{
				//TODO instanciate new ship
			}
		}

		resimulateFromTick++;
		while (resimulateFromTick <= mSectorTick)
		{
			updateShipsSystems(mSectorUpdateRate, resimulateFromTick);
			mDynamicWorld->stepSimulation(mSectorUpdateRate, 0, mSectorUpdateRate);

			resimulateFromTick++;
		}
	}
	else
	{
		updateShipsSystems(mSectorUpdateRate, mSectorTick);
		mDynamicWorld->stepSimulation(mSectorUpdateRate, 0, mSectorUpdateRate);
	}

	mSectorTick++;
}

/*void Sector::updateShots(float _deltaTime)
{
Shot* shot;
btVector3 oldPosition, newPosition;
std::vector<Shot*>& shotsList = mCurrentSector->getShots();
for(size_t i = 0; i < shotsList.size();)
{
shot = shotsList[i];

//Update life period
shotsList[i]->mTimeElapsed += _timeSinceLastFrame;
//Auto remove after life period
if(shotsList[i]->mTimeElapsed > shotsList[i]->getLifeTime())
{
shotsList[i]->destroy();
delete shotsList[i];
shotsList.erase(shotsList.begin() + i);
continue;
}

oldPosition = convert(shot->getSceneNode()->getPosition());
newPosition = convert(shot->getSceneNode()->getPosition()) + convert(shot->mSpeed * _timeSinceLastFrame);

btDiscreteDynamicsWorld::ClosestRayResultCallback closestRayResultCallback(oldPosition, newPosition);
mDynamicWorld->rayTest(oldPosition, newPosition, closestRayResultCallback);

if(closestRayResultCallback.hasHit())
{
StaticObject* shotObject = (StaticObject*)closestRayResultCallback.m_collisionObject->getCollisionShape()->getUserPointer();
//mLastShotTarget = shotObject->getName();
shotsList[i]->destroy();
delete shotsList[i];
shotsList.erase(shotsList.begin() + i);
}
else
{
shot->getSceneNode()->setPosition(convert(newPosition));
++i;
}
}
}*/

void ClientSector::updateShipsSystems(float _deltaTime, SectorTick _sectorTick)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateShipsSystems", "", false);

	const std::map<SectorTick, InputState >::const_iterator playerInputIt = mPlayerInputHistory.find(_sectorTick);
	if (playerInputIt != mPlayerInputHistory.end())
	{
		updateShipSystems((*playerInputIt).second, mPlayerShip, _deltaTime);
	}
	else
	{
		//TODO Log error
	}
}

void ClientSector::receivedSectorState(RakNet::BitStream& _data)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "receivedSectorState", "START", false);
	
	SectorTick sectorTick;
	_data.Read(sectorTick);

	//Read and deserialize state then stores it
	if (mLastReceivedSectorState.mSectorTick < sectorTick)
	{
		//Each client last input
		mLastReceivedSectorState.mClientInputMap.deserialize(_data);

		//Each ship
		mLastReceivedSectorState.mShips.clear();
		size_t shipsSize;
		_data.Read(shipsSize);

		//We write directly in ship history their state for tick mLastTickReceived
		RakNet::RakNetGUID rakNetId;
		for (size_t i = 0; i < shipsSize; ++i)
		{
			//Read client unique id
			_data.Read(rakNetId);

			//Read ship unique id
			UniqueId uniqueId;
			_data.Read(uniqueId);

			//Read ship state
			ShipState& shipState = mLastReceivedSectorState.mShips[rakNetId];
			shipState.deserialize(_data);
		}

		//Last ackonwledged input
		_data.Read(mLastReceivedSectorState.mLastAcknowledgedInput);

		//Set last received state as not simulated
		mLastReceivedSectorState.mSimulated = false;
	}
}

void ClientSector::addPlayerInputInHistory(const InputState& _inputState)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "addPlayerInputInHistory", "mSectorTick is " + StringUtils::toStr(mSectorTick), false);

	mPlayerInputHistory[mSectorTick] = _inputState;

	//Check that we don't have too much history
	/*if((*mPlayerInputHistory.begin()).first < mSectorTick - (mMaxInputRewind * 2))
	{
	SectorTick oldestAllowedInput = mSectorTick - mMaxInputRewind;
	while((*mPlayerInputHistory.begin()).first < oldestAllowedInput)
	mPlayerInputHistory.erase(mPlayerInputHistory.begin());
	}*/
}

void ClientSector::getPlayerInputAtTick(SectorTick _tick, InputState& _inputState)
{
	//Get last valid player input from _tick
	std::map<SectorTick, InputState>::const_iterator foundInput = mPlayerInputHistory.find(_tick);
	if (foundInput != mPlayerInputHistory.end())
		_inputState = (*foundInput).second;
}
