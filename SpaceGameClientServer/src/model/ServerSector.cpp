#include "model/ServerSector.h"

#include "model/StaticObject.h"
#include "model/GameSettings.h"
#include "model/InputState.h"
#include "model/ObjectPart.h"

#include "controller/SectorController.h"

#include "utils/StringUtils.h"
#include "utils/OgreUtils.h"
#include "utils/OgreBulletConvert.h"
#include "utils/BulletDebugDraw.h"

#include "manager/LoggerManager.h"
#include "manager/InputHistoryManager.h"

#include "network/ServerNetworkService.h"

#include "OgreSceneNode.h"

#include "RakNetTypes.h"
#include "BitStream.h"

namespace
{
	const std::string LOG_CLASS_TAG = "ServerSector";
}

UniqueId ServerSector::sUniqueId = -1;

void ServerSector::instantiateClientShip(const RakNet::RakNetGUID& _id, Ship& _ship, const Ogre::Quaternion& _orientation, const Ogre::Vector3& _position, UniqueId& _shipUniqueId)
{
	std::map<RakNet::RakNetGUID, Ship*>::const_iterator foundItem = mShips.find(_id);
	if (foundItem == mShips.end())
	{
		_shipUniqueId = getNextUniqueId();
		_ship.init(mSceneManager, mDynamicWorld, _shipUniqueId);
		_ship.instantiateObject();
		_ship.forceWorldTransform(btTransform(convert(_orientation), convert(_position)));
		mShips[_id] = &_ship;
		mUsersIds.insert(_id);

		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "instantiateClientShip", "_shipUniqueId : " + StringUtils::toStr(_shipUniqueId) + "; (GUID)_id : " + std::string(_id.ToString()) + "; _shipId : " + _ship.getName(), false);
	}
	else
	{
		LoggerManager::getInstance().logE(LOG_CLASS_TAG, "instantiateClientShip", "A ship already exists for _id : " + std::string(_id.ToString()));
		assert(false);
	}
}

void ServerSector::updateSector(const InputHistoryManager& _inputHistoryManager)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "START", false);

	//Update all clients ship systems
	std::list<ShotSettings> outputShots;
	updateShipsSystems(mSectorUpdateRate, _inputHistoryManager, outputShots);
	addShotObjects(outputShots);

	//Step physical simulation
	mDynamicWorld->stepSimulation(mSectorUpdateRate, 0, mSectorUpdateRate);
	
	//Sector state broadcasting
	RakNet::BitStream bitStream;
	this->serialize(bitStream);
	ServerNetworkService::getInstance().broadcastSector(mUsersIds, bitStream, _inputHistoryManager);

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "END", false);
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

void ServerSector::updateShipsSystems(float _deltaTime, const InputHistoryManager& _inputHistoryManager, std::list<ShotSettings>& _outputShots)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateShipsSystems", "START", false);

	for (std::map<RakNet::RakNetGUID, Ship*>::const_iterator shipIt = mShips.begin(), shipItEnd = mShips.end(); shipIt != shipItEnd; ++shipIt)
	{
		RakNet::RakNetGUID clientId = (*shipIt).first;
		Ship* clientShip = (*shipIt).second;
		InputState& clientInput = InputState();

		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateShipsSystems", "Updating ship systems for GUID : " + StringUtils::toStr(clientId.ToString()), false);

		//If we find input for the client we use it, else we use default
		//With no update from client, input is unchanged, meaning that a key pressed is still pressed until client says i not anymore
		const std::map<RakNet::RakNetGUID, SectorTick>::const_iterator foundClientInput = _inputHistoryManager.getNextInputToUse().find(clientId);
		if (foundClientInput != _inputHistoryManager.getNextInputToUse().end())
		{
			LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateShipsSystems", "Found input for tick " + StringUtils::toStr((*foundClientInput).second), false);
			_inputHistoryManager.getInputForTick(clientId, (*foundClientInput).second, clientInput);
		}
		else
		{
			LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateShipsSystems", "NO input was found", false);
		}

		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateShipsSystems", "input is : " + clientInput.getDebugString(), false);

		clientShip->updateSystems(clientInput, _deltaTime, _outputShots);
		clientShip->updateForces();
	}
	
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateShipsSystems", "END", false);
}

void ServerSector::serialize(RakNet::BitStream& _bitStream) const
{
	//Serialize ships
	_bitStream.Write(mShips.size());
	const std::map<RakNet::RakNetGUID, Ship*>::const_iterator shipsItEnd = mShips.end();
	for (std::map<RakNet::RakNetGUID, Ship*>::const_iterator shipsIt = mShips.begin(); shipsIt != shipsItEnd; ++shipsIt)
	{
		_bitStream.Write((*shipsIt).first);
		(*shipsIt).second->serialize(_bitStream);
	}

	//TODO dynamic objects
	//TODO shots
}

void ServerSector::updateSectorView()
{
	for (std::map<RakNet::RakNetGUID, Ship*>::iterator shipIt = mShips.begin(), shipItEnd = mShips.end(); shipIt != shipItEnd; ++shipIt)
	{
		(*shipIt).second->updateView();
	}
}