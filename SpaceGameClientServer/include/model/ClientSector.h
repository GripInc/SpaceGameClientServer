#ifndef _CLIENT_SECTOR_H_
#define _CLIENT_SECTOR_H_

#include "OgreVector3.h"

#include "model/Sector.h"
#include "model/StaticObject.h"
#include "model/PlanetObject.h"
#include "model/Shot.h"
#include "model/DynamicObject.h"
#include "model/Ship.h"
#include "model/ClientsInputMap.h"

#include "btBulletDynamicsCommon.h"

#include <string>
#include <list>

#include "SpaceGameTypes.h"

class SectorSettings;
class ShipSettings;
class ShipInputHandler;

namespace Ogre
{
	class SceneManager;
}

namespace RakNet
{
	class BitStream;
}

class ClientSector : public Sector
{
public:
	ClientSector(const std::string& _sectorName, Ogre::SceneManager* _sceneManager, float _sectorUpdateRate)
		: Sector(_sectorName, _sceneManager, _sectorUpdateRate)
	{}

	void setFirstTick(SectorTick _firstTick) { mSectorTick = _firstTick; }

	//void instantiateShip(const std::string& _shipId, const Ogre::Quaternion& _orientation, const Ogre::Vector3& _position, RakNet::RakNetGUID _rakNetGUID);
	void instantiatePlayerShip(Ship& _playerShip, const Ogre::Quaternion& _orientation, const Ogre::Vector3& _position, UniqueId _uniqueId, RakNet::RakNetGUID _rakNetGUID, Ogre::SceneNode* _cameraSceneNode);

	//Update function
	void updateSector(ShipInputHandler& _shipInputHandler);

	//For debug only
	const Ship* getPlayerShip() const { return mPlayerShip; }
	Ship* getPlayerShip() { return mPlayerShip; }

	//Unique id generator
	//Client regenerated unique ids are negative value, until server tells client wich unique it should use (positive one)
	UniqueId getTemporaryNextUniqueId() const { return sTemporaryUniqueId--; }

	void receivedSectorState(RakNet::BitStream& _data);

protected:
	static UniqueId sTemporaryUniqueId;

	//Sector objects
	Ship* mPlayerShip = nullptr; ///< Usefull to test special cases

	//Updates
	void updateShipsSystems(float _deltaTime, SectorTick _sectorTick);
	
	//Input handling
	ClientsInputMap mLastClientsInput;
	std::map<SectorTick, InputState> mPlayerInputHistory;
	void addPlayerInputInHistory(const InputState& _inputState);
	void getPlayerInputAtTick(SectorTick _tick, InputState& _inputState);
	unsigned int mMaxInputRewind = 0;

	//Stored last received sector state
	struct SectorState
	{
		SectorTick mSectorTick = 0;
		ClientsInputMap mClientInputMap;
		std::map<RakNet::RakNetGUID, ShipState> mShips;
		SectorTick mLastAcknowledgedInput = 0;
		bool mSimulated = true;
	} mLastReceivedSectorState;
};

#endif //_CLIENT_SECTOR_H_