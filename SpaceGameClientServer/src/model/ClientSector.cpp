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

#include "network/ClientNetworkService.h"

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

void ClientSector::updateSector(SectorTick _sectorTick, const std::list<InputState>& _playerInputHistory)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "START at tick " + StringUtils::toStr(_sectorTick), false);

	if (!mLastReceivedSectorState.mSimulated && mLastReceivedSectorState.mSectorTick > 0)
	{
		mLastReceivedSectorState.mSimulated = true;

		SectorTick resimulateFromTick = mLastReceivedSectorState.mSectorTick;

		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "resimulateFromTick is " + StringUtils::toStr(resimulateFromTick), false);

		//Set ship states as received from server
		for (std::map<RakNet::RakNetGUID, ShipState>::const_reference pair : mLastReceivedSectorState.mShips)
		{
			//Find the ship in sector
			std::map<RakNet::RakNetGUID, Ship*>::const_iterator foundShip = mShips.find(pair.first);
			if (foundShip != mShips.end())
			{
				LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "Setting ship state at tick : " + StringUtils::toStr(resimulateFromTick) + " for ship id " + StringUtils::toStr(pair.first.ToString()), false);

				Ship* ship = (*foundShip).second;
				ship->setState(pair.second);
			}
			else
			{
				//TODO instanciate new ship
			}
		}

		std::list<ShotSettings> outputShots;
		resimulateFromTick++;
		while (resimulateFromTick <= _sectorTick)
		{
			LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "Resimulate tick " + StringUtils::toStr(resimulateFromTick), false);

			updatePlayerShipSystems(mSectorUpdateRate, resimulateFromTick, _playerInputHistory, outputShots);
			addShotObjects(outputShots);
			mDynamicWorld->stepSimulation(mSectorUpdateRate, 0, mSectorUpdateRate);
			saveSectorState(resimulateFromTick);

			resimulateFromTick++;
		}
	}
	else
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "mLastReceivedSectorState.mSimulated == true", false);
		
		std::list<ShotSettings> outputShots;
		updatePlayerShipSystems(mSectorUpdateRate, _sectorTick, _playerInputHistory, outputShots);
		addShotObjects(outputShots);
		mDynamicWorld->stepSimulation(mSectorUpdateRate, 0, mSectorUpdateRate);
		saveSectorState(_sectorTick);
	}
	
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "END", false);
}

void ClientSector::updateSectorView(float _elapsedTime, SectorTick _sectorTick)
{
	for (std::map<RakNet::RakNetGUID, Ship*>::iterator shipIt = mShips.begin(), shipItEnd = mShips.end(); shipIt != shipItEnd; ++shipIt)
	{
		(*shipIt).second->updateView(_sectorTick, _elapsedTime, mSectorUpdateRate);
	}

	//TODO other entities
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

void ClientSector::updatePlayerShipSystems(float _deltaTime, SectorTick _sectorTick, const std::list<InputState>& _playerInputHistory, std::list<ShotSettings>& _outputShots)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updatePlayerShipSystems", "START", false);

	bool anInputHasBeenFound = false;
	InputState inputState;
	for (std::list<InputState>::const_reference inputStateRef : _playerInputHistory)
	{
		if (inputStateRef.mTick == _sectorTick)
		{
			inputState = inputStateRef;
			anInputHasBeenFound = true;
			break;
		}
	}

	if (anInputHasBeenFound)
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updatePlayerShipSystems", "input was found for tick " + StringUtils::toStr(_sectorTick), false);
		mPlayerShip->updateSystems(inputState, _deltaTime, _outputShots);
		mPlayerShip->updateForces();
	}
	else
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updatePlayerShipSystems", "ERROR, no input found", false);

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updatePlayerShipSystems", "END", false);
}

void ClientSector::storeReceivedSectorState(const std::map<RakNet::RakNetGUID, ShipState>& _shipStates, SectorTick _lastSimulatedInput)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "storeReceivedSectorState", "START", false);
	
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "storeReceivedSectorState", "_lastSimulatedInput is " + StringUtils::toStr(_lastSimulatedInput), false);
	mLastReceivedSectorState.mSectorTick = _lastSimulatedInput;
	mLastReceivedSectorState.mShips = _shipStates;
	mLastReceivedSectorState.mSimulated = false;

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "storeReceivedSectorState", "END", false);
}
