#ifndef _SERVER_SECTOR_H_
#define _SERVER_SECTOR_H_

#include "OgreVector3.h"

#include "model/Sector.h"
#include "model/StaticObject.h"
#include "model/PlanetObject.h"
#include "model/Shot.h"
#include "model/DynamicObject.h"
#include "model/Ship.h"

#include "RakNetTypes.h"
#include "btBulletDynamicsCommon.h"
#include "SpaceGameTypes.h"
#include "manager/InputHistoryManager.h"

#include <string>
#include <list>

class BulletDebugDraw;

class SectorSettings;
class ShipSettings;
class InputState;

namespace Ogre
{
	class SceneManager;
}

class ServerSector : public Sector
{
public:
	ServerSector(Ogre::SceneManager* _sceneManager, float _sectorUpdateRate, unsigned int _maxSectorTickRewindAmount)
		: Sector(_sceneManager, _sectorUpdateRate),
		mClientsInput(_maxSectorTickRewindAmount)
	{}
	
	//Add dynamic objects and instantiate them
	void instantiateClientShip(const RakNet::RakNetGUID& _id, Ship& _ship, const std::string& _shipId, const Ogre::Quaternion& _orientation, const Ogre::Vector3& _position, UniqueId& _shipUniqueId, SectorTick& _sectorTick);

	//Update function
	void updateSector();

	//Unique id generator
	UniqueId getNextUniqueId() const { return sUniqueId++; }

	//Serialize
	void serialize(RakNet::BitStream& _bitStream) const;

	//Add input for a client
	void addInput(const RakNet::RakNetGUID& _id, SectorTick _tick, const InputState& _clientInput);

protected:
	static UniqueId sUniqueId;

	//Users in sector
	std::set<RakNet::RakNetGUID> mUsersIds;

	//Update ships systems
	void updateShipsSystems(float _deltaTime, const ClientsInputMap& _clientsInputMap);

	//Simulate from oldest acceptable input to the last one
	//Return true if a rewind was performed
	bool simulateWorldForClientsHistory();

	//Clients input history of the sector
	InputHistoryManager mClientsInput;
};

#endif //_SERVER_SECTOR_H_