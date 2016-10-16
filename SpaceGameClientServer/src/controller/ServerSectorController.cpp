#include "controller/ServerSectorController.h"

#include "model/ServerSector.h"

#include "view/SectorView.h"

#include "manager/LoggerManager.h"

namespace
{
	const std::string LOG_CLASS_TAG = "SectorController";
}

void ServerSectorController::initSector(const std::string& _sectorName, Ogre::SceneManager* _sceneManager, float _sectorUpdateRate)
{
	if (mCurrentSector != NULL)
	{
		LoggerManager::getInstance().logI(LOG_CLASS_TAG, "createSector", "Deleting sector because it was not NULL", false);
		delete mCurrentSector;
	}

	mCurrentSector = new ServerSector(_sectorName, _sceneManager, _sectorUpdateRate);
}

void ServerSectorController::instanciateSectorObjects()
{
	mCurrentSector->instantiateObjects();
}

void ServerSectorController::instantiateClientShip(const RakNet::RakNetGUID& _id, Ship& _ship, const Ogre::Vector3& _position, const Ogre::Quaternion& _orientation, UniqueId& _shipUniqueId, SectorTick& _sectorTick)
{
	return mCurrentSector->instantiateClientShip(_id, _ship, _orientation, _position, _shipUniqueId, _sectorTick);
}

void ServerSectorController::updateSector()
{
	mCurrentSector->updateSector(); 
}

void ServerSectorController::addInput(const RakNet::RakNetGUID& _id, SectorTick _tick, const InputState& _clientInput)
{
	//TODO retrieve the sector the client is in
	//Add input in this sector input history

	//For now:
	mCurrentSector->addInput(_id, _tick, _clientInput);
}

void ServerSectorController::switchDisplayDebug()
{
	mCurrentSector->switchDisplayDebug(); 
}

void ServerSectorController::switchDisplay()
{
	mCurrentSector->switchDisplay(); 
}