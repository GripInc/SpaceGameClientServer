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

	//Add neutral input
	mPlayerInputHistory[mSectorTick] = InputState();
}

void ClientSector::updateSector(ShipInputHandler& _shipInputHandler)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "START: mSectorTick is : " + StringUtils::toStr(mSectorTick), false);

	if (_shipInputHandler.getHasInputChanged())
		addPlayerInputInHistory(_shipInputHandler.mInputState);

	if (mDoNeedRewindData.mDoNeedRewindFlag)
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "Need rewind flag is up.", false);
		reSimulateWorldFromTick(mDoNeedRewindData.mLastTickReceived);
	}
	else
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "Need rewind flag is down.", false);
		updateShipsSystems(mSectorUpdateRate, mSectorTick);
		mDynamicWorld->stepSimulation(mSectorUpdateRate, 1, mSectorUpdateRate);
	}

	//Reset rewind flag
	mDoNeedRewindData.mDoNeedRewindFlag = false;
	mDoNeedRewindData.mLastTickReceived = 0;

	saveSectorState();

	//Send player inputs to server only in space mode and only if changed
	_shipInputHandler.sendInputToServer(mSectorTick);

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "BEFORE Updating mSectorTick : " + StringUtils::toStr(mSectorTick), false);
	mSectorTick++;
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "AFTER Updating mSectorTick : " + StringUtils::toStr(mSectorTick), false);

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "END: mSectorTick is : " + StringUtils::toStr(mSectorTick), false);

	/////////////////////////////////
	//Old stuff

	//Update physics
	/*mTickAccumulator += _deltaTime;
	while( mTickAccumulator > WORLD_SIMULATION_RATE )
	{
	//updateShipsSystems(WORLD_SIMULATION_RATE);

	//TODO do it for every ship
	updateShipSystems(WORLD_SIMULATION_RATE, mPlayerShip);

	mDynamicWorld->stepSimulation(WORLD_SIMULATION_RATE, 10);

	saveSectorState();

	//Send player inputs to server only in space mode and only if changed
	_shipInputHandler.sendInputToServer(mSectorTick);

	mSectorTick++;
	mTickAccumulator -= WORLD_SIMULATION_RATE;
	}*/
}

void ClientSector::reSimulateWorldFromTick(SectorTick _tick)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "reSimulateWorldFromTick", "START at tick " + StringUtils::toStr(_tick), false);

	//Set sector state to currentSectorTick
	setSectorState(_tick);

	while (_tick <= mSectorTick)
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "reSimulateWorldFromTick", "ReSimulate tick " + StringUtils::toStr(_tick), false);

		updateShipsSystems(mSectorUpdateRate, _tick);
		mDynamicWorld->stepSimulation(mSectorUpdateRate, 1, mSectorUpdateRate);

		_tick++;
	}
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

	std::map<RakNet::RakNetGUID, Ship*>::iterator shipIt = mShips.begin();
	const std::map<RakNet::RakNetGUID, Ship*>::iterator shipItEnd = mShips.end();
	for (; shipIt != shipItEnd; ++shipIt)
	{
		Ship* clientShip = (*shipIt).second;
		InputState& clientInput = InputState();

		if (clientShip == mPlayerShip)
		{
			getPlayerInputAtTick(_sectorTick, clientInput);
		}
		else
		{
			RakNet::RakNetGUID clientId = (*shipIt).first;
			std::map<RakNet::RakNetGUID, InputState>::iterator foundClientInput = mLastClientsInput.find(clientId);
			if (foundClientInput != mLastClientsInput.end())
			{
				clientInput = (*foundClientInput).second;
			}
			else
			{
				LoggerManager::getInstance().logE(LOG_CLASS_TAG, "updateShipsSystems", "No input found for clientId : " + std::string(clientId.ToString()));
				assert(false);
			}
		}

		//What was that for??
		//if(!clientShip)
		//	continue;

		updateShipSystems(clientInput, clientShip, _deltaTime);
	}
}

void ClientSector::receivedSectorState(RakNet::BitStream& _data)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "receivedSectorState", "START", false);

	SectorTick sectorTick;
	_data.Read(sectorTick);

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "receivedSectorState", "sectorTick is : " + StringUtils::toStr(sectorTick), false);
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "receivedSectorState", "mDoNeedRewindData.mLastTickReceived is : " + StringUtils::toStr(mDoNeedRewindData.mLastTickReceived), false);

	//We just drop out of date sector dumps
	if (mDoNeedRewindData.mLastTickReceived >= sectorTick)
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "receivedSectorState", "Dropping state.", false);
		return;
	}
	else
	{
		mDoNeedRewindData.mLastTickReceived = sectorTick;
		mDoNeedRewindData.mDoNeedRewindFlag = true;
	}

	//Each client last input
	//mLastClientsInput.clear();
	//_data.Read(mLastClientsInput);

	//Each ship
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
		ShipState shipState;
		shipState.deserialize(_data);

		//Find the ship in sector
		std::map<RakNet::RakNetGUID, Ship*>::const_iterator foundShip = mShips.find(rakNetId);
		if (foundShip != mShips.end())
		{
			Ship* ship = (*foundShip).second;
			ship->overrideSavedState(sectorTick, shipState);
		}
		else
		{
			LoggerManager::getInstance().logW(LOG_CLASS_TAG, "receivedSectorState", "No ship found for raknet id : " + std::string(rakNetId.ToString()));
		}
	}

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "receivedSectorState", "END", false);
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
	else
	{
		std::map<SectorTick, InputState>::const_reverse_iterator clientsInputByTickIt(foundInput); //Converting it to reverse_it point to the next element in reverse view it = 5 -> reverseIt = 4
		if (clientsInputByTickIt != mPlayerInputHistory.rend())
			_inputState = (*clientsInputByTickIt).second;
		else
		{
			LoggerManager::getInstance().logE(LOG_CLASS_TAG, "getPlayerInputAtTick", "No valid player input found from _tick " + StringUtils::toStr(_tick));
			assert(false);
		}
	}
}