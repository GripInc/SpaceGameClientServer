#include "model/ServerSector.h"

#include "model/StaticObject.h"
#include "model/GameSettings.h"
#include "model/InputState.h"
#include "model/ObjectPart.h"
#include "model/HardPoint.h"

#include "controller/SectorController.h"

#include "utils/StringUtils.h"
#include "utils/OgreUtils.h"
#include "utils/OgreBulletConvert.h"
#include "utils/BulletDebugDraw.h"

#include "manager/LoggerManager.h"

#include "network/ServerNetworkService.h"

#include "OgreSceneNode.h"

#include "RakNetTypes.h"
#include "BitStream.h"

namespace
{
	const std::string LOG_CLASS_TAG = "ServerSector";
}

UniqueId ServerSector::sUniqueId = -1;

void ServerSector::instantiateClientShip(const RakNet::RakNetGUID& _id, Ship& _ship, const Ogre::Quaternion& _orientation, const Ogre::Vector3& _position, UniqueId& _shipUniqueId, SectorTick& _sectorTick)
{
	std::map<RakNet::RakNetGUID, Ship*>::const_iterator foundItem = mShips.find(_id);
	if (foundItem == mShips.end())
	{
		_sectorTick = mSectorTick;
		_shipUniqueId = getNextUniqueId();
		_ship.init(mSceneManager, mDynamicWorld, _shipUniqueId);
		_ship.instantiateObject();
		_ship.forceWorldTransform(btTransform(convert(_orientation), convert(_position)));
		mShips[_id] = &_ship;
		mUsersIds.insert(_id);

		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "instantiateClientShip", "_sectorTick : " + StringUtils::toStr(_sectorTick) + "; _shipUniqueId : " + StringUtils::toStr(_shipUniqueId) + "; (GUID)_id : " + std::string(_id.ToString()) + "; _shipId : " + _ship.getName(), false);

		//We want at least one neutral input
		addInput(_id, mSectorTick, InputState());
	}
	else
	{
		LoggerManager::getInstance().logE(LOG_CLASS_TAG, "instantiateClientShip", "A ship already exists for _id : " + std::string(_id.ToString()));
		assert(false);
	}
}

void ServerSector::updateSector()
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "START: mSectorTick is : " + StringUtils::toStr(mSectorTick), false);

	//resimulate from this input
	//lastSimulatedInputTick = 0 if no rewind was performed, so clients were up to date at last broadcast
	//lastSimulatedInputTick != 0 if a rewind was performed, so clients need new state at last input tick simulated (!= mSectorTick)
	bool needBroadcast = simulateWorldForClientsHistory();

	saveSectorState();

	if (needBroadcast)
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "Broadcasting to clients mSectorTick is : " + StringUtils::toStr(mSectorTick), false);

		//Sector state and last input broadcasting
		RakNet::BitStream bitStream;
		this->serialize(bitStream);
		ServerNetworkService::getInstance().broadcastSector(mUsersIds, bitStream);
	}

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "BEFORE Updating mSectorTick : " + StringUtils::toStr(mSectorTick), false);
	mSectorTick++;
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "AFTER Updating mSectorTick : " + StringUtils::toStr(mSectorTick), false);

	mClientsInput.update(mSectorTick);

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "END: mSectorTick is : " + StringUtils::toStr(mSectorTick), false);
}

bool ServerSector::simulateWorldForClientsHistory()
{
	ClientsInputMap clientsInputMap;
	SectorTick currentSectorTick = 0;
	//The check for oldest possible rewind is done here
	bool needRewind = mClientsInput.getOldestUnsimulatedInput(clientsInputMap, currentSectorTick);

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "simulateWorldForClientsHistory", "Oldest unsimulated is " + StringUtils::toStr(currentSectorTick), false);

	//Rewind to a state then simulate until last input
	if (needRewind)
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "simulateWorldForClientsHistory", "Need rewind.", false);

		//Set sector state to currentSectorTick
		setSectorState(currentSectorTick);

		while (true)
		{
			LoggerManager::getInstance().logI(LOG_CLASS_TAG, "simulateWorldForClientsHistory", "Resimulate tick " + StringUtils::toStr(currentSectorTick), false);

			updateShipsSystems(mSectorUpdateRate, clientsInputMap);
			mDynamicWorld->stepSimulation(mSectorUpdateRate, 1, mSectorUpdateRate);

			currentSectorTick++;

			if (currentSectorTick > mSectorTick)
				break;

			mClientsInput.getLastInputForAllClients(currentSectorTick, clientsInputMap);
		}
	}
	else
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "simulateWorldForClientsHistory", "Don't need rewind. Simulate current tick : " + StringUtils::toStr(mSectorTick), false);
		updateShipsSystems(mSectorUpdateRate, clientsInputMap);
		mDynamicWorld->stepSimulation(mSectorUpdateRate, 1, mSectorUpdateRate);
	}

	return needRewind;
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

void ServerSector::updateShipsSystems(float _deltaTime, const ClientsInputMap& _clientsInputMap)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateShipsSystems", "", false);

	for (std::map<RakNet::RakNetGUID, Ship*>::const_iterator shipIt = mShips.begin(), shipItEnd = mShips.end(); shipIt != shipItEnd; ++shipIt)
	{
		RakNet::RakNetGUID clientId = (*shipIt).first;
		Ship* clientShip = (*shipIt).second;
		InputState& clientInput = InputState();

		//If we find input for the client we use it, else we use default
		//With no update from client, input is unchanged, meaning that a key pressed is still pressed until client says i not anymore
		//TODO best practice seems to be "send as much as input as we can to get pressed/unpressed states quite exact"
		ClientsInputMap::const_iterator foundClientInput = _clientsInputMap.find(clientId);
		if (foundClientInput != _clientsInputMap.end())
		{
			clientInput = (*foundClientInput).second;
		}
		else
		{
			LoggerManager::getInstance().logE(LOG_CLASS_TAG, "updateShipsSystems", "No input found for clientId : " + std::string(clientId.ToString()));
			assert(false);
		}

		updateShipSystems(clientInput, clientShip, _deltaTime);
	}
}

void ServerSector::serialize(RakNet::BitStream& _bitStream) const
{
	_bitStream.Write(mSectorTick);

	//serialize last input for each client
	ClientsInputMap lastClientsInputMap;
	//mClientsInput.getLastInputForAllClients(mSectorTick, lastClientsInputMap);
	//_bitStream.Write(lastClientsInputMap);

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

void ServerSector::addInput(const RakNet::RakNetGUID& _id, SectorTick _tick, const InputState& _clientInput)
{
	mClientsInput.addInput(_id, _tick, _clientInput);
}