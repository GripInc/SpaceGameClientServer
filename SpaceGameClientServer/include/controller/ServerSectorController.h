#ifndef _SERVER_SECTOR_CONTROLLER_H_
#define _SERVER_SECTOR_CONTROLLER_H_

#include "controller/SectorController.h"

#include <string>

#include "OgrePrerequisites.h"
#include "RakNetTypes.h"

#include "SpaceGameTypes.h"

class InputState;
class ServerSector;
class Ship;

class ServerSectorController : public SectorController
{
public:
	///Client input buffer
	class ClientInputBuffer
	{
	public:
		SectorTick mLastSimulatedInputTick = 0;
		std::map<SectorTick, InputState> mInputBuffer;
	};

	//Return the sector tick when the ship was created
	void instantiateClientShip(const RakNet::RakNetGUID& _id, Ship& _ship, const Ogre::Vector3& _position, const Ogre::Quaternion& _orientation, UniqueId& _shipUniqueId, SectorTick& _sectorTick);

	//Getters
	ServerSector* getCurrentSector() { return mCurrentSector; }

	//Update function
	void updateSector();

	//Add input for a client in a sector
	void addInput(const RakNet::RakNetGUID& _id, SectorTick _tick, const InputState& _clientInput);

	virtual void switchDisplayDebug() override;
	virtual void switchDisplay() override;

protected:
	ServerSector* mCurrentSector = nullptr;

	virtual void initSector(const std::string& _sectorName, Ogre::SceneManager* _sceneManager, float _sectorUpdateRate) override;
	virtual void instanciateSectorObjects() override;
};

#endif //_SERVER_SECTOR_CONTROLLER_H_