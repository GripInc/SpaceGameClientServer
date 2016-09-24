#ifndef _SECTOR_CONTROLLER_H_
#define _SECTOR_CONTROLLER_H_

#include <string>

#include "OgrePrerequisites.h"
#include "LinearMath/btScalar.h"

#include "model/Sector.h"

class SectorView;
class Ship;
class ShipInputHandler;

namespace RakNet
{
	class BitStream;
}

class SectorController
{
public:
	SectorController() 
		: mCurrentSector(NULL),
		mSectorView(NULL)
	{}

	void createSector(const std::string& _sectorName, Ogre::SceneManager* _sceneManager, float _sectorUpdateRate, SectorTick _startingSectorTick);
	void instantiatePlayerShip(Ship& _playerShip, const std::string& _shipId, const Ogre::Vector3& _position, const Ogre::Quaternion& _orientation, UniqueId _uniqueId, RakNet::RakNetGUID _rakNetGUID) { mCurrentSector->instantiatePlayerShip(_playerShip, _shipId, _orientation, _position, _uniqueId, _rakNetGUID); }

	//Getters
	Sector* getCurrentSector() { return mCurrentSector; }
	const Ship* getPlayerShip() const { return mCurrentSector->getPlayerShip(); }
	Ship* getPlayerShip() { return mCurrentSector->getPlayerShip(); }

	//Update function
	void updateSector(ShipInputHandler& _shipInputHandler) { mCurrentSector->updateSector(_shipInputHandler); }

	//Received sector update from server
	void receivedSectorState(RakNet::BitStream& _data) const;

	//DEBUG
	std::string mLastShotTarget;
	std::string mLastCollidedPart0;
	std::string mLastCollidedPart1;
	float mLastCollisionSpeed = 0.f;
	void switchDisplayDebug() { mCurrentSector->switchDisplayDebug(); }
	void switchDisplay() { mCurrentSector->switchDisplay(); }

protected:
	SectorView* mSectorView = nullptr;
	Sector* mCurrentSector = nullptr;
};

#endif //_SECTOR_CONTROLLER_H_