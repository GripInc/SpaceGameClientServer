#ifndef _SERVER_SECTOR_CONTROLLER_H_
#define _SERVER_SECTOR_CONTROLLER_H_

#include "controller/SectorController.h"

#include "manager/InputHistoryManager.h"

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
	static const unsigned int SERVER_INPUT_BUFFER_LENGTH;

	//Return the sector tick when the ship was created
	void instantiateClientShip(const RakNet::RakNetGUID& _id, Ship& _ship, const Ogre::Vector3& _position, const Ogre::Quaternion& _orientation, UniqueId& _shipUniqueId);

	//Getters
	ServerSector* getCurrentSector() { return mCurrentSector; }

	//Update function
	void updateSector();

	//Add inputs for a client in a sector
	void addInputs(const RakNet::RakNetGUID& _id, const std::list<InputState>& _clientInputs);

	virtual void switchDisplayDebug() override;
	virtual void switchDisplay() override;

protected:
	ServerSector* mCurrentSector = nullptr;

	virtual void initSector(const std::string& _sectorName, Ogre::SceneManager* _sceneManager, float _sectorUpdateRate) override;
	virtual void instanciateSectorObjects() override;

	//Clients input history of the sector
	InputHistoryManager mClientsInput;
};

#endif //_SERVER_SECTOR_CONTROLLER_H_