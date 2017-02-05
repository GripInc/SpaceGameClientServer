#ifndef _CLIENT_SECTOR_CONTROLLER_H_
#define _CLIENT_SECTOR_CONTROLLER_H_

#include "controller/SectorController.h"

#include "manager/StateManager.h"

#include <string>

#include "OgrePrerequisites.h"
#include "RakNetTypes.h"

#include "SpaceGameTypes.h"

class ClientSector;
class Ship;
class ShipInputHandler;
class InputState;

namespace RakNet
{
	class BitStream;
}

class ClientSectorController : public SectorController
{
public:
	void instantiatePlayerShip(Ship& _playerShip, const Ogre::Vector3& _position, const Ogre::Quaternion& _orientation, UniqueId _uniqueId, RakNet::RakNetGUID _rakNetGUID, Ogre::SceneNode* _cameraSceneNode);

	//Getters
	ClientSector* getCurrentSector() { return mCurrentSector; }
	const Ship* getPlayerShipConst() const;
	Ship* getPlayerShip();

	//Update function
	void updateSector(const ShipInputHandler& _shipInputHandler);
	void updateSectorView(float _elapsedTime);

	//Received sector update from server
	void receivedSectorState(const std::map<RakNet::RakNetGUID, ShipState>& _shipStates, SectorTick _lastAcknowledgedInput, SectorTick _lastSimulatedInput);

	virtual void switchDisplayDebug() override;
	virtual void switchDisplay() override;

protected:
	ClientSector* mCurrentSector = nullptr;
	
	SectorTick mLastAcknowledgedInput = 0;
	SectorTick mLastSimulatedInput = 0;
	std::list<InputState> mPlayerInputHistory;

	virtual void initSector(const std::string& _sectorName, Ogre::SceneManager* _sceneManager, float _sectorUpdateRate) override;
	virtual void instanciateSectorObjects() override;
};

#endif //_CLIENT_SECTOR_CONTROLLER_H_