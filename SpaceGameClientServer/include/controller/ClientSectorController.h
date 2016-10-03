#ifndef _CLIENT_SECTOR_CONTROLLER_H_
#define _CLIENT_SECTOR_CONTROLLER_H_

#include "controller/SectorController.h"

#include <string>

#include "OgrePrerequisites.h"
#include "RakNetTypes.h"

#include "SpaceGameTypes.h"

class ClientSector;
class Ship;
class ShipInputHandler;

namespace RakNet
{
	class BitStream;
}

class ClientSectorController : public SectorController
{
public:
	void setFirstTick(SectorTick _firstTick);
	void instantiatePlayerShip(Ship& _playerShip, const Ogre::Vector3& _position, const Ogre::Quaternion& _orientation, UniqueId _uniqueId, RakNet::RakNetGUID _rakNetGUID, Ogre::SceneNode* _cameraSceneNode);

	//Getters
	ClientSector* getCurrentSector() { return mCurrentSector; }
	const Ship* getPlayerShipConst() const;
	Ship* getPlayerShip();

	//Update function
	void updateSector(ShipInputHandler& _shipInputHandler);

	//Received sector update from server
	void receivedSectorState(RakNet::BitStream& _data) const;

	virtual void switchDisplayDebug() override;
	virtual void switchDisplay() override;

protected:
	ClientSector* mCurrentSector = nullptr;

	virtual void initSector(const std::string& _sectorName, Ogre::SceneManager* _sceneManager, float _sectorUpdateRate) override;
	virtual void instanciateSectorObjects() override;
};

#endif //_CLIENT_SECTOR_CONTROLLER_H_