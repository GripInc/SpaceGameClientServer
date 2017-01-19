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
	}
	else
	{
		LoggerManager::getInstance().logE(LOG_CLASS_TAG, "instantiateClientShip", "A ship already exists for _id : " + std::string(_id.ToString()));
		assert(false);
	}
}

void ServerSector::updateSector()
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "mSectorTick : " + StringUtils::toStr(mSectorTick), false);

	//Update all clients ship systems with clientsInputMap
	updateShipsSystems(mSectorUpdateRate, mClientsInput.getLastInputReceivedFromClient());

	//Step physical simulation
	mDynamicWorld->stepSimulation(mSectorUpdateRate, 0, mSectorUpdateRate);
	saveSectorState(mSectorTick);
	
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateSector", "Broadcasting to clients mSectorTick is : " + StringUtils::toStr(mSectorTick), false);
	
	//Sector state broadcasting
	RakNet::BitStream bitStream;
	this->serialize(bitStream);
	ServerNetworkService::getInstance().broadcastSector(mUsersIds, bitStream, mClientsInput.getLastInputReceivedFromClient());

	updateShipsView();

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

void ServerSector::updateShipsSystems(float _deltaTime, const ClientsInputMap& _clientsInputMap)
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
		const ClientsInputMap::const_iterator foundClientInput = _clientsInputMap.find(clientId);
		if (foundClientInput != _clientsInputMap.end())
		{
			LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateShipsSystems", "Input was found with tick " + StringUtils::toStr((*foundClientInput).second.mTick), false);
			clientInput = (*foundClientInput).second;
		}
		else
		{
			LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateShipsSystems", "NO input was found", false);
		}

		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "updateShipsSystems", "input is : " + clientInput.getDebugString(), false);

		updateShipSystems(clientInput, clientShip, _deltaTime);
	}
}

void ServerSector::serialize(RakNet::BitStream& _bitStream) const
{
	_bitStream.Write(mSectorTick);

	//serialize last input for each client
	ClientsInputMap lastClientsInputMap = mClientsInput.getLastInputReceivedFromClient();

	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "serialize", "Serialized at tick "+ StringUtils::toStr(mSectorTick) +":\n" + lastClientsInputMap.getDebugString(), false);

	lastClientsInputMap.serialize(_bitStream);

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

void ServerSector::addInput(const RakNet::RakNetGUID& _id, const InputState& _clientInput)
{
	LoggerManager::getInstance().logI(LOG_CLASS_TAG, "addInput", "", false);

	mClientsInput.addInput(_id, _clientInput);
}

void ServerSector::updateShipsView()
{
	for (std::map<RakNet::RakNetGUID, Ship*>::iterator shipIt = mShips.begin(), shipItEnd = mShips.end(); shipIt != shipItEnd; ++shipIt)
	{
		(*shipIt).second->updateView(mSectorTick, 1.f, 1.f);
	}
}